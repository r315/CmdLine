#ifndef _cmdsump_h_
#define _cmdsump_h_

#ifdef __cplusplus
extern "C"
{
#endif

#include "console.h"
#include "stimer.h"

    typedef enum
    {
        /**
         * Resets the device.
         *
         * Should be sent 5 times when the receiver status is unknown. (It could be
         * waiting for up to four bytes of pending long command data.)
         */
        SUMP_RESET = 0x00,
        /**
         * Arms the trigger.
         */
        SUMP_RUN = 0x01,
        /**
         * Asks for device identification.
         *
         * The device will respond with four bytes. The first three ("SLA") identify
         * the device. The last one identifies the protocol version which is currently
         * either "0" or "1"
         */
        SUMP_ID = 0x02,

        /**
         * Get metadata.
         *
         * In response, the device sends a series of 1-byte keys, followed by data
         * pertaining to that key. The series ends with the key 0x00. The system can be
         * extended with new keys as more data needs to be reported.
         *
         * Type 0 Keys (null-terminated string, UTF-8 encoded):
         *
         * 0x00 Not used, key means end of metadata
         * 0x01 Device name (e.g. "Openbench Logic Sniffer v1.0", "Bus Pirate v3b")
         * 0x02 Version of the FPGA firmware
         * 0x03 Ancillary version (PIC firmware)
         *
         * Type 1 Keys (32-bit unsigned integer):
         *
         * 0x20 Number of usable probes
         * 0x21 Amount of sample memory available (bytes)
         * 0x22 Amount of dynamic memory available (bytes)
         * 0x23 Maximum sample rate (Hz)
         * 0x24 Protocol version (see below) [*]
         *
         * Type 2 Keys (8-bit unsigned integer):
         *
         * 0x40 Number of usable probes (short)
         * 0x41 Protocol version (short)
         *
         * [*]
         *
         * The protocol version key holds a 4-stage version, one per byte, where the
         * MSB holds the major version number. As of the first release to support this
         * metadata command, the protocol version should be 2. This would be encoded
         * as 0x00000002.
         */
        SUMP_DESC = 0x04,

        /**
         * Put transmitter out of pause mode.
         *
         * It will continue to transmit captured data if any is pending. This command
         * is being used for XON/XOFF flow control.
         */
        SUMP_XON = 0x11,

        /**
         * Put transmitter in pause mode.
         *
         * It will stop transmitting captured data. This command is being used for
         * XON/XOFF flow control.
         */
        SUMP_XOFF = 0x13,

        /**
         * Set Divider.
         *
         * When x is written, the sampling frequency is set to f = clock / (x + 1)
         *
         *          LSB                  MSB
         * 10000000 XXXXXXXXXXXXXXXXXXXXXXXX????????
         *          ||||||||||||||||||||||||
         *          ++++++++++++++++++++++++----------- Divider
         */
        SUMP_DIV = 0x80,

        /**
         * Set Read & Delay Count.
         *
         * Read Count is the number of samples (divided by four) to read back from
         * memory and sent to the host computer. Delay Count is the number of samples
         * (divided by four) to capture after the trigger fired. A Read Count bigger
         * than the Delay Count means that data from before the trigger match will be
         * read back. This data will only be valid if the device was running long
         * enough before the trigger matched.
         *
         *          LSB          MSB LSB          MSB
         * 10000001 XXXXXXXXXXXXXXXX YYYYYYYYYYYYYYYY
         *          |||||||||||||||| ||||||||||||||||
         *          |||||||||||||||| ++++++++++++++++--- Delay Count
         *          ++++++++++++++++-------------------- Read Count
         */
        SUMP_CNT = 0x81,

        /**
         * Set Flags.
         *
         * Sets the following flags:
         *
         * - demux: Enables the demux input module. (Filter must be off.)
         * - filter: Enables the filter input module. (Demux must be off.)
         * - channel groups: Disable channel group. Disabled groups are excluded from
         *                   data transmissions. This can be used to speed up transfers.
         *                   There are four groups, each represented by one bit.
         *                   Starting with the least significant bit of the channel
         *                   group field channels are assigned as follows: 0-7, 8-15,
         *                   16-23, 24-31
         * - external: Selects the clock to be used for sampling. If set to 0, the
         *             internal clock divided by the configured divider is used, and if
         *             set to 1, the external clock will be used. (filter and demux are
         *             only available with internal clock)
         * - inverted: When set to 1, the external clock will be inverted before being
         *             used. The inversion causes a delay that may cause problems at
         *             very high clock rates. This option only has an effect with
         *             external set to 1.
         *
         * 10000010 ABCCCCDE ????????????????????????
         *          ||||||||
         *          |||||||+---------------------------- Demux (1: Enable)
         *          ||||||+----------------------------- Filter (1: Enable)
         *          ||++++------------------------------ Channel Groups (1: Disable)
         *          |+---------------------------------- External (1: Enable)
         *          +----------------------------------- Inverted (1: Enable)
         */
        SUMP_FLAGS = 0x82,

        /**
         * Set Trigger Values.
         *
         * Defines which trigger values must match. In parallel mode each bit
         * represents one channel, in serial mode each bit represents one of the last
         * 32 samples of the selected channel. The opcodes refer to stage 0-3 in the
         * order given below. (Protocol version 0 only supports stage 0.)
         *
         *          LSB                          MSB
         * 1100xx00 XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
         *          ||||||||||||||||||||||||||||||||
         *          ++++++++++++++++++++++++++++++++--- Trigger Mask
         */
        SUMP_TRIG = 0xC0,

        /**
         * Set trigger mask.
         *
         * Defines which values individual bits must have. In parallel mode each bit
         * represents one channel, in serial mode each bit represents one of the last
         * 32 samples of the selected channel. The opcodes refer to stage 0-3 in the
         * order given above. (Protocol version 0 only supports stage 0.)
         *
         *          LSB                          MSB
         * 1100xx01 XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
         *          ||||||||||||||||||||||||||||||||
         *          ++++++++++++++++++++++++++++++++--- Trigger Mask
         */
        SUMP_TRIG_VALS = 0xC1,

        /**
         * Not used, key means end of metadata.
         */
        SUMP_METADATA_END = 0x00,

        /**
         * Device name (e.g. "Openbench Logic Sniffer v1.0", "Bus Pirate v3b").
         */
        SUMP_METADATA_DEVICE_NAME = 0x01,

        /**
         * Version of the FPGA firmware.
         */
        SUMP_METADATA_FPGA_VERSION = 0x02,

        /**
         * Ancillary version (PIC firmware).
         */
        SUMP_METADATA_ANCILLARY_VERSION = 0x03,

        /**
         * Number of usable probes.
         */
        SUMP_METADATA_USABLE_PROBES_NUMBER = 0x20,

        /**
         * Amount of sample memory available (bytes).
         */
        SUMP_METADATA_SAMPLE_MEMORY_AVAILABLE = 0x21,

        /**
         * Amount of dynamic memory available (bytes).
         */
        SUMP_METADATA_DYNAMIC_MEMORY_AVAILABLE = 0x22,

        /**
         * Maximum sample rate (Hz).
         */
        SUMP_METADATA_MAXIMUM_SAMPLE_RATE = 0x23,

        /**
         * Protocol version.
         */
        SUMP_METADATA_PROTOCOL_VERSION = 0x24,

        /**
         * Number of usable probes (short).
         */
        SUMP_METADATA_USABLE_PROBES_SHORT_NUMBER = 0x40,

        /**
         * Protocol version (short).
         */
        SUMP_METADATA_PROTOCOL_SHORT_VERSION = 0x41
    } sump_protocol_commands;

#define SUMP_COMMAND_BUFFER_LENGTH 5
/**
 * Default timer period value for polling probes.
 */
#define BP_DEFAULT_TIMER_PERIOD 0x00000640


/**
 * SUMP protocol version the Bus Pirate supports.
 */
#define BP_SUMP_PROTOCOL_VERSION 2

    /**
     * Incoming command processing states.
     */
    typedef enum
    {
        /** Waiting for a command to arrive on the serial port. */
        RX_COMMAND_IDLE = 0,
        /** A long command was received, and parameters need to be acquired. */
        RX_COMMAND_PARAMETERS,
        /** A fully formed command was received and is being processed. */
        RX_COMMAND_PROCESS
    } sump_command_state_t;

    /**
     * SUMP command storage structure.
     */
    typedef struct
    {
        /** Command bytes storage buffer. */
        uint8_t bytes[SUMP_COMMAND_BUFFER_LENGTH];
        /** How many parameter bytes still need to be obtained. */
        uint8_t left;
        /** How many parameter bytes have been obtained so far. */
        uint8_t count;
    } __attribute__((packed)) sump_command_t;

    class CmdSump : public ConsoleCommand
    {
        Console *console;

    public:
        void init(void *params) { console = static_cast<Console *>(params); }

        void help(void);
        char execute(int argc, char **argv);

        CmdSump() : ConsoleCommand("sump") {}

    private:
        stimer_t m_timer;
    };

#ifdef __cplusplus
}
#endif

#endif