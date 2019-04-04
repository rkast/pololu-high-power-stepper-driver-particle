// Copyright Pololu Corporation.  For more information, see http://www.pololu.com/

/*! \file DRV8711.h
 *
 * This is the main header file for the DRV8711 library, a library for
 * controllering the DRV8711 micro-stepping stepper motor driver.
 *
 * For more information about this library, see:
 *
 *   https://github.com/pololu/drv8711-arduino
 *
 * That is the main repository for this library.
 *
 */

#pragma once

#include <stdint.h>
#include <Arduino.h>
#include <SPI.h>

/*! This class provides low-level functions for reading and writing from the SPI
 * interface of an DRV8711 micro-stepping stepper motor driver.
 *
 * Most users should use the DRV8711 class, which provides a higher-level
 * interface, instead of this class. */
class DRV8711SPI
{
public:
    /*! Configures this object to use the specified pin as a slave select pin.
     * You must use a slave select pin; the DRV8711 requires it. */
    void init(uint8_t slaveSelectPin) {
        ssPin = slaveSelectPin;
        digitalWrite(ssPin, LOW);
        pinMode(ssPin, OUTPUT);
    }

    /*! Reads the register at the given address and returns its raw value. */
    uint16_t readReg(uint8_t address)
    {
        selectChip();
        uint16_t dataOut = transfer((0x8 | address) << 12);
        deselectChip();
        return dataOut;
    }

    /*! Writes the specified value to a register. */
    void writeReg(uint8_t address, uint16_t value)
    {
        selectChip();
        transfer((address << 12) | value);

        // The CS line must go low after writing for the value to actually take
        // effect.
        deselectChip();
    }

private:

    SPISettings settings = SPISettings(500000, MSBFIRST, SPI_MODE0);

    uint16_t transfer(uint16_t value)
    {
        return SPI.transfer16(value);
    }

    void selectChip()
    {
        digitalWrite(ssPin, HIGH);
        SPI.beginTransaction(settings);
    }

    void deselectChip()
    {
       SPI.endTransaction();
       digitalWrite(ssPin, LOW);
    }

    uint8_t ssPin;
};

/*! This class provides high-level functions for controlling an DRV8711
 *  micro-stepping motor driver.
 *
 * It provides access to all the features of the DRV8711 SPI interface
 * except the watchdog timer. */
class DRV8711
{
public:
    /*! The default constructor. */
    DRV8711()
    {
        // All settings set to default configurations on power-reset
        ctrl = 0xC10;
        torque = 0x1FF;
        off = 0x30;
        blank = 0x80;
        decay = 0x110;
        status = 0x0;
    }

    /*! Addresses of control and status registers. */
    enum regAddr
    {
        CTRL = 0x00,
        TORQUE = 0x01,
        OFF = 0x02,
        BLANK = 0x03,
        DECAY = 0x04,
        //STALL = 0x05,
        //DRIVE = 0x06,
        STATUS = 0x07,
    };

	/*! Possible arguments to setGain(). */
	enum isgain
	{
		Gain5 = 5,
		Gain10 = 10,
		Gain20 = 20,
		Gain40 = 40,
	};

	/*! Possible arguments to setDeadTime(). */
	enum deadTime
	{
		DeadTime400ns = 400,
		DeadTime450ns = 450,
		DeadTime650ns = 650,
		DeadTime850ns = 850,
	};

    /*! Possible arguments to setStepMode(). */
    enum stepMode
    {
        MicroStep256 = 256,
        MicroStep128 = 128,
        MicroStep64 = 64,
        MicroStep32 = 32,
        MicroStep16 = 16,
        MicroStep8 = 8,
        MicroStep4 = 4,
        MicroStep2 = 2,
        MicroStep1 = 1,
    };

    /*! Configures this object to use the specified pin as a slave select pin.
     * You must use a slave select pin; the DRV8711 requires it. */
    void init(uint8_t slaveSelectPin)
    {
        driver.init(slaveSelectPin);
    }

    /*! Sets the ENBL bit to 1, enabling the driver. */
    void enableDriver()
    {
        ctrl |= 1;
        writeCTRL();
    }

    /*! Sets the ENBL bit to 0, disabling the driver. */
    void disableDriver()
    {
        ctrl &= 0;
        writeCTRL();
    }

	/*! Sets the RDIR bit: 0 sets direction by DIR pin and 1 sets direction
	 * inverse of DIR pin. */
	void flipDirection()
	{
		ctrl = ctrl ^ (1 << 1);
		writeCTRL();
	}

	/*! Sets the RSTEP bit to 1: indexer will advance one step, automatically
	 * cleared after write. */
	void step()
	{
		ctrl |= (1 << 2);
    writeCTRL();
	}

    /*! Configures the driver to have the specified stepping mode.
     *
     * This affects many things about the performance of the motor, including
     * how much the output moves for each step taken and how much current flows
     * through the coils in each stepping position.
     *
     * The argument to this function should be one of the members of the
     * #stepMode enum.
     *
     * If an invalid stepping mode is passed to this function, then it selects
     * 1/4 micro-step, which is the driver's default. */
    void setStepMode(uint16_t mode)
    {
        // Pick 1/4 micro-step by default.
        uint16_t sm = 0b0010;

        // The order of these cases matches the order in Table 3 of the
        // DRV8711 datasheet.
        switch(mode)
        {
        case MicroStep1: sm = 0b0000; break;
        case MicroStep2: sm = 0b0001; break;
        case MicroStep4: sm = 0b0010; break;
        case MicroStep8: sm = 0b0011; break;
        case MicroStep16: sm = 0b0100; break;
        case MicroStep32: sm = 0b0101; break;
        case MicroStep64: sm = 0b0110; break;
        case MicroStep128: sm = 0b0111; break;
        case MicroStep256: sm = 0b1000; break;
        }

        ctrl = (ctrl & 0b111110000111) | (sm << 3);
        writeCTRL();
    }

	/*! Sets EXSTALL bit to 0, detecting stall externally.
	 * By default, EXSTALL bit set to 0: internal stall detect. */
	void setExternalStallDetection()
	{
		ctrl |= (1 << 7);
		writeCTRL();
	}

	/*! Sets EXSTALL bit to 1, detecting stall internally.
	 * By default, EXSTALL bit set to 0: internal stall detect. */
	void setInternalStallDetection()
	{
		ctrl &= ~(1 << 7);
		writeCTRL();
	}

	/*! Sets ISGAIN bit to a gain of: 5, 10, 20, or 40. */
	void setGain(uint8_t gain)
	{
		// Pick gain of 20 by default.
		uint16_t ag = 0b10;

		switch(gain)
		{
		case Gain5: ag = 0b00; break;
		case Gain10: ag = 0b01; break;
		case Gain20: ag = 0b10; break;
		case Gain40: ag = 0b11; break;
		}

		ctrl = (ctrl & 0b110011111111) | (ag << 8);
		writeCTRL();
	}

	/*! Sets DTIME bit to time: 400 ns, 450 ns, 650 ns, or 850 ns. */
	void setDeadTime(uint8_t dTime)
	{
		// Pick dead time of 850 ns by default.
		uint16_t dt = 0b11;

		switch(dTime)
		{
		case DeadTime400ns: dt = 0b00; break;
		case DeadTime450ns: dt = 0b01; break;
		case DeadTime650ns: dt = 0b10; break;
		case DeadTime850ns: dt = 0b11; break;
		}

		ctrl = (ctrl & 0b110011111111) | (dt << 10);
		writeCTRL();
	}

	/*! Sets TORQUE bits [7:0] in TORQUE register.  See equation in
	 * datasheet. */
	void setTorque(uint8_t torqueValue)
	{
		torque = (torque & 0b11100000000) | torqueValue;
		writeTORQUE();
	}

	/*! Sets TOFF bits [7:0] in 500 ns increments in OFF register.  See
	 * datasheet for more details. */
	void setOffTime(uint8_t offTime)
	{
		off = (off & 0b100000000) | offTime;
		writeOFF();
	}

	/*! Sets PWMMODE bit: 0 - uses internal indexer and 1 - bypasses indexer,
	 * using xINx inputs to control outputs. See datasheet for more
	 * information. */
	void setPWMMode(uint16_t pwmMode)
	{
		off = off ^ (1 << 8);
		writeOFF();
	}

protected:

    uint16_t ctrl, torque, off, blank, decay, status;

    /*! Writes the cached value of the CTRL register to the device. */
    void writeCTRL()
    {
        driver.writeReg(CTRL, ctrl);
    }

    /*! Writes the cached value of the TORQUE register to the device. */
    void writeTORQUE()
    {
        driver.writeReg(TORQUE, torque);
    }

    /*! Writes the cached value of the OFF register to the device. */
    void writeOFF()
    {
        driver.writeReg(OFF, off);
    }

    /*! Writes the cached value of the BLANK register to the device. */
    void writeBLANK()
    {
        driver.writeReg(BLANK, blank);
    }

    /*! Writes the cached value of the DECAY register to the device. */
    void writeDECAY()
    {
        driver.writeReg(DECAY, decay);
    }

    /*! Writes the cached value of the STALL register to the device. */
    //void writeSTALL()
    //{
    //    driver.writeReg(STALL, stall);
    //}

    /*! Writes the cached value of the DRIVE register to the device. */
    //void writeDRIVE()
    //{
    //    driver.writeReg(DRIVE, drive);
    //}

    /*! Writes the cached value of the STATUS register to the device. */
    void writeSTATUS()
    {
        driver.writeReg(STATUS, status);
    }

public:
    /*! This object handles all the communication with the DRV8711.  It is
     * only marked as public for the purpose of testing this library; you should
     * not use it in your code. */
    DRV8711SPI driver;
};