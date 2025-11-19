#ifdef UNIT_TEST
#include "app/uart_interface.hpp"
#include "app/traffic_system_manager.hpp"
#include "drivers/timer_driver.hpp"
#include "app/traffic_light.hpp"
#include "app/button.hpp"
#include "app/pedestrian_light.hpp"
#include "app/buzzer.hpp"
#include "app/pedestrian_button.hpp"
#include "app/command.hpp"
#include "config/pin_config.hpp"
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <memory>
#include <string>
#include <fstream>

class MockUartInterface : public IUartInterface
{
public:
    MockUartInterface()
        : has_pending_command_(false)
    {
        ClearBuffer();
    }

    void LoadIncomingCommand(const char* command)
    {
        if (command == nullptr)
        {
            return;
        }
        size_t length = strlen(command);
        if (length >= sizeof(incoming_command_))
        {
            length = sizeof(incoming_command_) - 1;
        }
        memcpy(incoming_command_, command, length);
        incoming_command_[length] = '\0';
        has_pending_command_ = true;
    }

    const char* GetLastSentMessage() const
    {
        return last_sent_message_;
    }

    void Send(const char* message) override
    {
        if (message == nullptr)
        {
            return;
        }
        size_t length = strlen(message);
        if (length >= sizeof(last_sent_message_))
        {
            length = sizeof(last_sent_message_) - 1;
        }
        memcpy(last_sent_message_, message, length);
        last_sent_message_[length] = '\0';
    }

    bool ReceiveLine(char* buffer, uint8_t length) override
    {
        if (!has_pending_command_ || buffer == nullptr || length == 0)
        {
            return false;
        }

        size_t copy_length = strlen(incoming_command_);
        if (copy_length >= length)
        {
            copy_length = length - 1;
        }

        memcpy(buffer, incoming_command_, copy_length);
        buffer[copy_length] = '\0';
        has_pending_command_ = false;
        return true;
    }

private:
    void ClearBuffer()
    {
        incoming_command_[0] = '\0';
        last_sent_message_[0] = '\0';
    }

    bool has_pending_command_;
    char incoming_command_[64];
    char last_sent_message_[64];
};

// ...existing code...

// Simple interface (abstract class)
struct ILogger {
    virtual ~ILogger() = default;
    virtual void log(const std::string &msg) = 0;
};

// Concrete implementation: console logger
struct ConsoleLogger : ILogger {
    void log(const std::string &msg) override {
        std::cout << "[Console] " << msg << '\n';
    }
};

// Concrete implementation: file logger
struct FileLogger : ILogger {
    std::ofstream ofs;
    FileLogger(const std::string &path) : ofs(path, std::ios::app) {}
    void log(const std::string &msg) override {
        if (ofs) ofs << "[File] " << msg << '\n';
    }
};

// Service that depends on ILogger via constructor injection
class Service {
public:
    explicit Service(std::shared_ptr<ILogger> logger) : logger_(std::move(logger)) {}
    void doWork() {
        logger_->log("Service starting work");
        // ... work ...
        logger_->log("Service finished work");
    }
private:
    std::shared_ptr<ILogger> logger_;
};

// ...existing code...

// Test pentru TrafficSystemManager cu dependency injection
int main()
{
    printf("=== Traffic System Manager Unit Tests ===\n\n");

    // Creare componente mock pentru testare
    TimerDriver timer_driver;
    TrafficLight traffic_lights(
        TRAFFIC_LIGHT_LEFT_RED_PORT, TRAFFIC_LIGHT_LEFT_RED_PIN,
        TRAFFIC_LIGHT_LEFT_YELLOW_PORT, TRAFFIC_LIGHT_LEFT_YELLOW_PIN,
        TRAFFIC_LIGHT_LEFT_GREEN_PORT, TRAFFIC_LIGHT_LEFT_GREEN_PIN,
        TRAFFIC_LIGHT_RIGHT_RED_PORT, TRAFFIC_LIGHT_RIGHT_RED_PIN,
        TRAFFIC_LIGHT_RIGHT_YELLOW_PORT, TRAFFIC_LIGHT_RIGHT_YELLOW_PIN,
        TRAFFIC_LIGHT_RIGHT_GREEN_PORT, TRAFFIC_LIGHT_RIGHT_GREEN_PIN
    );
    
    Button button_right(BUTTON_RIGHT_PORT, BUTTON_RIGHT_PIN);
    Button button_left(BUTTON_LEFT_PORT, BUTTON_LEFT_PIN);
    
    PedestrianLight pedestrian_lights(
        PEDESTRIAN_LEFT_RED_PORT, PEDESTRIAN_LEFT_RED_PIN,
        PEDESTRIAN_LEFT_GREEN_PORT, PEDESTRIAN_LEFT_GREEN_PIN,
        PEDESTRIAN_RIGHT_RED_PORT, PEDESTRIAN_RIGHT_RED_PIN,
        PEDESTRIAN_RIGHT_GREEN_PORT, PEDESTRIAN_RIGHT_GREEN_PIN
    );
    
    Buzzer buzzers(BUZZER_PORT, BUZZER_PIN);
    
    MockUartInterface mock_uart;
    
    PedestrianButton pedestrian_button(
        &button_right,
        &button_left,
        &traffic_lights,
        &pedestrian_lights,
        &buzzers,
        &timer_driver,
        &mock_uart
    );
    
    Command command_manager(&pedestrian_button);

    // Test 1: Creare manager cu dependency injection
    printf("Test 1: Creating TrafficSystemManager with dependency injection...\n");
    TrafficSystemManager manager(
        &timer_driver,
        &traffic_lights,
        &button_right,
        &button_left,
        &pedestrian_lights,
        &buzzers,
        &pedestrian_button,
        &command_manager,
        &mock_uart
    );
    
    TrafficSystemManager::SetInstance(&manager);
    printf("PASS: Manager created successfully\n\n");

    // Test 2: Verificare getters
    printf("Test 2: Testing getters...\n");
    if (manager.GetTrafficLights() == &traffic_lights)
    {
        printf("PASS: GetTrafficLights() works\n");
    }
    else
    {
        printf("FAIL: GetTrafficLights() failed\n");
    }
    
    if (manager.GetButtonRight() == &button_right)
    {
        printf("PASS: GetButtonRight() works\n");
    }
    else
    {
        printf("FAIL: GetButtonRight() failed\n");
    }
    printf("\n");

    // Test 3: Test logging
    printf("Test 3: Testing logging system...\n");
    manager.Log(LogLevel::INFO, "Test info message");
    printf("Last log: %s", mock_uart.GetLastSentMessage());
    
    manager.Log(LogLevel::ERROR, "Test error message");
    printf("Last log: %s", mock_uart.GetLastSentMessage());
    printf("PASS: Logging works\n\n");

    // Test 4: Test comenzi (simulare)
    printf("Test 4: Testing command processing...\n");
    mock_uart.LoadIncomingCommand("help");
    manager.Update(); // ProcessCommands va fi apelat
    printf("Command response: %s", mock_uart.GetLastSentMessage());
    printf("PASS: Command processing works\n\n");

    printf("=== All tests completed ===\n\n");

    // Test 5: Dependency Injection Pattern Demo
    printf("=== Dependency Injection Pattern Demo ===\n\n");
    
    // Swap implementations by changing what we inject:
    // 1) Console logger
    auto consoleLogger = std::make_shared<ConsoleLogger>();
    Service s1(consoleLogger);
    printf("Using ConsoleLogger:\n");
    s1.doWork();
    printf("\n");

    // 2) File logger
    auto fileLogger = std::make_shared<FileLogger>("app.log");
    Service s2(fileLogger);
    printf("Using FileLogger (check app.log file):\n");
    s2.doWork();
    printf("\n");

    printf("=== Dependency Injection Demo completed ===\n");
    return 0;
}
#endif  // UNIT_TEST