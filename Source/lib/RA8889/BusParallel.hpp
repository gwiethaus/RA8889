#include <Bus.hpp>

class Bus_Parallel : public IBus {
    Config _cfg;
public:
    void config(const Config& cfg) override;
protected:
    void writeCommand(uint8_t cmd) override;
    void writeData(uint8_t data) override;
    void readData(void) override;
};