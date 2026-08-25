#include "cmdsump.h"
#include "debug.h"
#include "board.h"
#include "wdt.h"
#include "clock.h"
#include "la.h"


/**
 * SUMP_ID response buffer, advertise ourselves as a Logic Sniffer.
 *
 * According to the specifications on sump.org, this should either be 'SLA0' or
 * 'SLA1', however both Logic Sniffer (http://ols.lxtreme.nl) and
 * sigrok (https://sigrok.org) will not handle the device otherwise.
 */
static const char SUMP_DEVICE_ID[] = {'1', 'A', 'L', 'S'};

/**
 * SUMP metadata information for the Bus Pirate device.
 */
static const char SUMP_METADATA[] = {
    /* Device name. */
    SUMP_METADATA_DEVICE_NAME, 'A', 'T', '3', '2', '\0',
    /* Sample memory (4096 bytes). */
    SUMP_METADATA_SAMPLE_MEMORY_AVAILABLE,
    (uint8_t)((uint32_t)SAMPLER_MEMORY_SIZE >> 24),
    (uint8_t)(((uint32_t)SAMPLER_MEMORY_SIZE >> 16) & 0xFF),
    (uint8_t)(((uint32_t)SAMPLER_MEMORY_SIZE >> 8) & 0xFF),
    (uint8_t)((uint32_t)SAMPLER_MEMORY_SIZE & 0xFF),
    /* Sample rate (1MHz). */
    SUMP_METADATA_MAXIMUM_SAMPLE_RATE,
    (uint8_t)((uint32_t)SAMPLER_MAX_SAMPLE_RATE >> 24),
    (uint8_t)(((uint32_t)SAMPLER_MAX_SAMPLE_RATE >> 16) & 0xFF),
    (uint8_t)(((uint32_t)SAMPLER_MAX_SAMPLE_RATE >> 8) & 0xFF),
    (uint8_t)((uint32_t)SAMPLER_MAX_SAMPLE_RATE & 0xFF),
    /* Number of probes (5). */
    SUMP_METADATA_USABLE_PROBES_SHORT_NUMBER, SAMPLER_NUM_OF_CHANNELS,
    /* Protocol version (v2). */
    SUMP_METADATA_PROTOCOL_SHORT_VERSION, BP_SUMP_PROTOCOL_VERSION,
    SUMP_METADATA_END
};


/**
 * The current state of the command processor.
 */
static sump_command_state_t sump_command_state = RX_COMMAND_IDLE;
/*
* The command storage buffer.
*
* No need to clear it first, as it will be properly initialized upon
* receiving a long (5 bytes) command.
*/
static sump_command_t command_buffer;

static bool sumpCommand(uint8_t input_byte)
{
    switch (sump_command_state) {
        /* No command bytes received yet, this is the first one. */
        case RX_COMMAND_IDLE:
            switch (input_byte) {
                /* Reset the analyzer. */
                case SUMP_RESET:
                    sampler_reset();
                return true;
                /* Send the device identification buffer. */
                case SUMP_ID:
                    host_serial->write(SUMP_DEVICE_ID, sizeof(SUMP_DEVICE_ID));
                break;
                /* Send device description. */
                case SUMP_DESC:
                    host_serial->write(SUMP_METADATA, sizeof(SUMP_METADATA));
                break;
                /* Arm the sampler. */
                case SUMP_RUN:
                    sampler_arm();
                break;
                /* Start/Stop data flow. */
                case SUMP_XON:
                case SUMP_XOFF:
                    /* Stop and resume are not supported yet. */
                break;
                /* It must be a long command then. */
                default:
                    /* Store the first byte. */
                    command_buffer.bytes[0] = input_byte;
                    /* Update counters. */
                    command_buffer.left = 4;
                    command_buffer.count = 0;
                    /* Update state. */
                    sump_command_state = RX_COMMAND_PARAMETERS;
                break;
            }
        break;
        /* Keep reading parameter data. */
        case RX_COMMAND_PARAMETERS:
            /* Update command count. */
            command_buffer.count++;
            /* Fill buffer. */
            command_buffer.bytes[command_buffer.count] = input_byte;
            /* Check whether the buffer is full or not. */
            if (command_buffer.count < command_buffer.left) {
                break;
            }

        /* Intentional fall-through. */

        /* Process the fully read command buffer. */
        case RX_COMMAND_PROCESS:
            switch (command_buffer.bytes[0]) {
                /* Set triggers. */
                case SUMP_TRIG:
                    sampler_set_triggers(command_buffer.bytes[1]);
                break;
                case SUMP_FLAGS:
                    /* @TODO: Fill this? */
                break;
                /* Read requested samples buffer size. */
                case SUMP_CNT: {
                    uint32_t nsamples =
                    (((command_buffer.bytes[2] << 8) + command_buffer.bytes[1]) + 1) * 4;
                    /* Clamp sample counter if more bytes are requested. */
                    if (nsamples > SAMPLER_MEMORY_SIZE) {
                        nsamples = SAMPLER_MEMORY_SIZE;
                    }
                    sampler_set_samples(nsamples);
                }
                break;

                case SUMP_DIV: {
                    /*
                    * Read the 24-bits period value and rescale from SUMP's
                    * own 100MHz frequency range
                    */
                    uint32_t div;
                    div = ((uint32_t)command_buffer.bytes[3] << 16) +
                             ((uint32_t)command_buffer.bytes[2] << 8) +
                             ((uint32_t)command_buffer.bytes[1] << 0);

                    sampler_set_rate(100000000UL / (div + 1));
                }
                break;
            }
            sump_command_state = RX_COMMAND_IDLE;
        break;
    }
    return false;
}

void CmdSump::help(void)
{
    console->println("Usage: sump <init|start>\n");
    console->println("\tinit,   Initialize sump and sample engine");
    console->println("\tstart,  Enter sump mode");
}

char CmdSump::execute(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    if(CMD_IS_PARM_1("init")){
        sampler_init();
        sampler_reset();
    }

    if(CMD_IS_PARM_1("start")){
        while(1){
            if(host_serial->available()){
                uint8_t input_byte;
                host_serial->read((char*)&input_byte, 1);
                sumpCommand(input_byte);
            }
            WDT_Reset();
            uint32_t acquired;
            if((acquired = sampler_acquire_samples()) > 0){
                /* Write captured samples out */
                host_serial->write((const char*)sampler_get_samples(), acquired);
            }
        }
    }

    if(CMD_IS_PARM_1("run")){
        sumpCommand(SUMP_RUN);
        sampler_acquire_samples();
        sampler_get_samples();
    }

    return CMD_OK;
}



