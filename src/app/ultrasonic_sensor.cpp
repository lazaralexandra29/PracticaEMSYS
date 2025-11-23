#include "app/ultrasonic_sensor.hpp"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// Counter volatile pentru microsecunde (incrementat în ISR)
static volatile uint32_t g_microsecond_counter = 0;

// ISR pentru Timer0 - incrementă counter-ul la fiecare microsecundă
ISR(TIMER0_COMPA_vect)
{
    g_microsecond_counter++;
}

UltrasonicSensor::UltrasonicSensor(volatile uint8_t* trig_port,
                                   uint8_t trig_pin,
                                   volatile uint8_t* echo_port,
                                   uint8_t echo_pin)
    : gpio_(),
      trig_pin_(trig_port, trig_pin),
      echo_pin_(echo_port, echo_pin)
{
}

void UltrasonicSensor::Init()
{
    gpio_.SetPinDirection(trig_pin_, PinDirection::OUT);
    gpio_.SetPinValue(trig_pin_, PinValue::LOW);
    gpio_.SetPinDirection(echo_pin_, PinDirection::IN);
    InitMicrosecondTimer();
}

void UltrasonicSensor::InitMicrosecondTimer()
{
    // Configurează Timer0 pentru tick-uri de 1µs
    // Presupunând 16MHz CPU:
    // Prescaler 8: 16MHz / 8 = 2MHz = 0.5µs per tick
    // OCR0A = 1: interrupt la fiecare 2 ticks = 1µs
    
    TCCR0A = 0;
    TCCR0B = 0;
    
    // Mod CTC (Clear Timer on Compare)
    TCCR0A |= (1 << WGM01);
    
    // Prescaler 8
    TCCR0B |= (1 << CS01);
    
    // OCR0A = 1 pentru interrupt la fiecare 2 ticks (1µs la 16MHz cu prescaler 8)
    // Pentru 8MHz: OCR0A = 0 (1 tick = 1µs cu prescaler 8)
    // Pentru 16MHz: OCR0A = 1 (2 ticks = 1µs cu prescaler 8)
    #if F_CPU == 16000000UL
        OCR0A = 1;  // 16MHz: 2 ticks = 1µs
    #elif F_CPU == 8000000UL
        OCR0A = 0;  // 8MHz: 1 tick = 1µs
    #else
        OCR0A = 1;  // Default pentru 16MHz
    #endif
    
    // Activează interrupt
    TIMSK0 |= (1 << OCIE0A);
    
    // Timer0 rulează continuu - nu oprește interrupts
    sei();
}

void UltrasonicSensor::StartMicrosecondTimer()
{
    // Nu resetează counter-ul global (ar afecta alte operații)
    // Doar folosim counter-ul existent pentru măsurători relative
}

void UltrasonicSensor::StopMicrosecondTimer()
{
    // Nu oprește timer-ul - rulează continuu
    // Doar pentru compatibilitate cu API-ul
}

uint32_t UltrasonicSensor::GetMicrosecondCounter()
{
    // Citește atomic counter-ul
    cli();
    uint32_t count = g_microsecond_counter;
    sei();
    return count;
}

void UltrasonicSensor::WaitMicroseconds(uint32_t us)
{
    // Pentru delay-uri foarte mici (2µs, 10µs), folosim timer hardware
    // dar nu blocăm complet - doar așteptăm counter-ul să se incrementeze
    uint32_t start = GetMicrosecondCounter();
    while ((GetMicrosecondCounter() - start) < us)
    {
        // Busy wait - dar nu blocăm cu delay, folosim timer hardware
        // Alte interrupts (Timer1, etc.) pot rula în timpul ăsta
    }
}

bool UltrasonicSensor::MeasureDistanceCm(uint16_t& distance_cm)
{
    // PASUL 1: Trimite pulsul de trigger
    // Pentru delay-uri foarte mici (2µs, 10µs) folosim delay normal
    // - sunt acceptabile pentru că sunt extrem de scurte
    gpio_.SetPinValue(trig_pin_, PinValue::LOW);
    _delay_us(kTriggerDelayUs);
    gpio_.SetPinValue(trig_pin_, PinValue::HIGH);
    _delay_us(kTriggerPulseUs);
    gpio_.SetPinValue(trig_pin_, PinValue::LOW);

    // PASUL 2: Așteaptă ca ECHO să devină HIGH (non-blocant)
    StartMicrosecondTimer();
    uint32_t start_time = GetMicrosecondCounter();
    PinValue pin_state = PinValue::LOW;
    
    while ((GetMicrosecondCounter() - start_time) < kEchoTimeoutUs)
    {
        gpio_.ReadPinValue(echo_pin_, pin_state);
        if (pin_state == PinValue::HIGH)
        {
            break;  // ECHO a devenit HIGH
        }
    }
    
    if (pin_state != PinValue::HIGH)
    {
        StopMicrosecondTimer();
        return false;  // Timeout - nu s-a primit răspuns
    }

    // PASUL 3: Măsoară cât timp ECHO rămâne HIGH (non-blocant)
    start_time = GetMicrosecondCounter();
    uint32_t pulse_length = 0;
    
    while ((GetMicrosecondCounter() - start_time) < kEchoTimeoutUs)
    {
        gpio_.ReadPinValue(echo_pin_, pin_state);
        if (pin_state == PinValue::LOW)
        {
            pulse_length = GetMicrosecondCounter() - start_time;
            break;  // ECHO a revenit la LOW
        }
    }
    
    StopMicrosecondTimer();

    if (pin_state != PinValue::LOW || pulse_length >= kEchoTimeoutUs)
    {
        return false;  // Timeout sau eroare
    }

    // PASUL 4: Calculează distanța
    // Formula: distanța = (timp * viteza_sunetului) / 2
    // Viteza sunetului = 343 m/s = 34300 cm/s = 0.0343 cm/µs
    // Deci: distanța (cm) = pulse_length (µs) * 0.0343 / 2
    // Simplificat: distanța (cm) ≈ pulse_length / 58
    // În cod: pulse_length / 10 (aproximare)
    distance_cm = static_cast<uint16_t>(pulse_length / 10);
    return true;
}

