
/* Includes */
#include "mbed.h"
#include <cstdio>
#include "HTS221Sensor.h"
#include "LPS22HBSensor.h"
#include "LSM6DSLSensor.h"
#include "lis3mdl_class.h"
#include "VL53L0X.h"

// objects for various sensors
static DevI2C devI2c(PB_11,PB_10);
static LPS22HBSensor press_temp(&devI2c);
static HTS221Sensor hum_temp(&devI2c);
static LSM6DSLSensor acc_gyro(&devI2c,0xD4,D4,D5); // high address
static LIS3MDL magnetometer(&devI2c, 0x3C);
static DigitalOut shutdown_pin(PC_6);
static VL53L0X range(&devI2c, &shutdown_pin, PC_7, 0x52);

UnbufferedSerial serial_port(USBTX, USBRX);

DigitalOut led_1(LED1);
DigitalOut led_2(LED2);
DigitalOut led_3(LED3);
char buff;


// functions to print sensor data
void print_t_rh(){
    float value1, value2;
    hum_temp.get_temperature(&value1);
    hum_temp.get_humidity(&value2);

    value1=value2=0;    
    press_temp.get_temperature(&value1);
    press_temp.get_pressure(&value2);
    printf("LPS22HB: [temp] %.2f C, [press] %.2f mbar\r\n", value1, value2);
}

void print_mag(){
    int32_t axes[3];
    magnetometer.get_m_axes(axes);
    printf("LIS3MDL [mag/mgauss]:    %6ld, %6ld, %6ld\r\n", axes[0], axes[1], axes[2]);

}

void print_accel(){
    int32_t axes[3];
    acc_gyro.get_x_axes(axes);
    printf("LSM6DSL [acc/mg]:        %6ld, %6ld, %6ld\r\n", axes[0], axes[1], axes[2]);
}

void print_gyro(){
    int32_t axes[3];
    acc_gyro.get_g_axes(axes);
    printf("LSM6DSL [gyro/mdps]:     %6ld, %6ld, %6ld\r\n", axes[0], axes[1], axes[2]);
}

void print_distance(){
    uint32_t distance;
    int status = range.get_distance(&distance);
    if (status == VL53L0X_ERROR_NONE) {
        printf("VL53L0X [mm]:            %6ld\r\n", distance);
    } else {
        printf("VL53L0X [mm]:                --\r\n");
    }
}

void readSerial(){ // was testing, trying to get print_accel() etc calling from with readSerial()
    if(serial_port.readable()){
        serial_port.read(&buff, 1);
        serial_port.write(&buff, 1);
        led_1 = 0;
        led_2 = 0;
        led_3 = 0;
        // printf("0x%X\r\n", serial_port.read(&buff, 1));

        // wait_us(1000);
        // switch (serial_port.read(&buff, 1)) {
        // switch (serial_port.write(&buff, 1)) {
        switch (buff) {
         case '1': 
            print_accel();
            led_1 = 0;
            led_2 = 0;
            led_3 = 1;
            break;
         case '2': 
            //print_accel();
            led_1 = 0;
            led_2 = 1;
            led_3 = 0;
            break;
         case '3': 
            // print_accel();
            led_1 = 0;
            led_2 = 1;
            led_3 = 1;
            break;
         case '4': 
            // print_accel();
            led_1 = 1;
            led_2 = 0;
            led_3 = 0;
            break;
         case '5': 
            // print_accel();
            led_1 = 1;
            led_2 = 0;
            led_3 = 1;
            break;
        case '6': 
            // print_accel();
            led_1 = 1;
            led_2 = 1;
            led_3 = 1;
            break;
        }  
        // strcmp( argc[i], "&") == 0     
        // if(buff == 'a'){
        //     printf("\n\r--- Reading sensor values ---\n\r"); ; 
        // } else {
        //     serial_port.write(&buff, 1);
        // }
    }
}

int main() {
    uint8_t id;
    float value1, value2;

    int32_t axes[3];

    hum_temp.init(NULL);

    press_temp.init(NULL);
    magnetometer.init(NULL);
    acc_gyro.init(NULL);

    range.init_sensor(0x52);

    hum_temp.enable();
    press_temp.enable();

    acc_gyro.enable_x();
    acc_gyro.enable_g();
  
    printf("\033[2J\033[20A");
    printf ("\r\n--- Starting new run ---\r\n\r\n");

    hum_temp.read_id(&id);
    printf("HTS221  humidity & temperature    = 0x%X\r\n", id);

    press_temp.read_id(&id);
    printf("LPS22HB pressure & temperature    = 0x%X\r\n", id);
    magnetometer.read_id(&id);
    printf("LIS3MDL magnetometer              = 0x%X\r\n", id);
    acc_gyro.read_id(&id);
    printf("LSM6DSL accelerometer & gyroscope = 0x%X\r\n", id);
    
    printf("\n\r--- Reading sensor values ---\n\r");
    print_t_rh();
    print_mag();
    print_accel();
    print_gyro();
    print_distance();
    printf("\r\n");

    // display instructions
    printf("Press 1: for Thermometer\n");
    printf("Press 2: for Magnetometer\n");
    printf("Press 3: for Accelerometer\n");
    printf("Press 4: for Gyroscope\n");
    printf("Press 5: for Distance\n");
    printf("Press 6: for all sensors\n");  
    printf("\r\n");

    led_1 = 1;
    led_2 = 1;
    led_3 = 1;

    serial_port.baud(9600);
    serial_port.format(
        /* bits */ 8,
        /* parity */ SerialBase::None,
        /* stop bit */ 1
    );
    
    while(1) {
        serial_port.read(&buff, 1);
        serial_port.write(&buff, 1);

        switch (buff) {
         case '1': 
            printf(": Temperature & Presssure: ");
            print_t_rh();  
            printf("\r\n");
            break;
         case '2': 
            printf(": Magnetometer: ");
            print_mag();   
            printf("\r\n"); 
            break;
         case '3': 
            printf(": Acceleration: ");
            print_accel();
            printf("\r\n");    
            break;
         case '4': 
            printf(": Gyroscope: ");
            print_gyro();  
            printf("\r\n");  
            break;
         case '5': 
            printf(": Distance: ");
            print_distance();
            printf("\r\n");
            break;
        case '6':                 
            printf("\n\r--- Reading sensor values ---\n\r"); ;
            print_t_rh();
            print_mag();
            print_accel();
            print_gyro();
            print_distance();
            printf("\r\n");
            break;
        }  

    // serial_port.attach(readSerial);
    
    // while(1) {
         wait_us(500000);
    }
}