  #define ANALOG_IN_PIN_ELEVATOR 32
  #define ANALOG_IN_PIN_AILERON 33
  #define ANALOG_IN_PIN_THROTTLE 34
  #define ANALOG_IN_PIN_RUDDER 35
  #define ANALOG_IN_PIN_AUX1 25  // Arm switch
  #define ANALOG_IN_PIN_AUX2 26  // Mode switch
  #define ANALOG_IN_PIN_AUX3 27  // gpio27
  #define ANALOG_IN_PIN_AUX4 12  // gpio12
  #define VOLTAGE_READ_PIN 36

extern bool powerChangeHasRun;

extern uint32_t tickTime;
extern uint16_t rates[];
// click deboucer
static uint32_t clickCurrentMicros = 0;

#define TEMPSTRINGLENGTH 400 // This is the max dialog size (80 characters * 5 lines)
                             // We could reduce this to ~240 on the 128x64 screens
                             // But only after all sprintf are replaced with snprintf
                             // Maybe move this to target_defs.h
extern char tempstring[TEMPSTRINGLENGTH];

void check_link_state(uint32_t currentMicros);
const char *hdr_str_cb(const void *data);
void crsfdevice_init();
void bt_handle(uint8_t value);
char tempstring[TEMPSTRINGLENGTH];

bool powerChangeHasRun = false;

uint32_t tickTime = 0;
uint32_t tickInterval = 2000000; // 2 sec. to check if rx or tx connect/disconnect

uint16_t rates[] = {0, 25, 50, 100, 150, 200};

// void crsfdevice_init()
// {
//     next_param = 1;
//     next_chunk = 0;
//     recv_param_ptr = recv_param_buffer;
//     params_loaded = 0;
//     // CBUF_Init(send_buf);
// }

void check_link_state(uint32_t currentMicros)
{

    dbout.printf("tick :: tx: %u rx: %u\n", txConected, rxConected);

    // for (size_t i = 0;crsf_devices[i].address; i++) dbout.printf("device address: 0x%x\n",crsf_devices[i].address);

    uint8_t tmp = LinkStatistics.rf_Mode;
    // if (MODULE_IS_ELRS) {
    if (txConected > 0)
    {
        if ((int)local_info.good_pkts == 0)
        {
            dbout.printf("get crsf link statistics\n");
            CRSF_get_elrs_info(ELRS_ADDRESS);
        }
        else if ((int)local_info.good_pkts != (int)rates[tmp] && rxConected > 0)
        {
            dbout.printf("update crsf link statistics\n");
            CRSF_get_elrs_info(ELRS_ADDRESS);
        }

        if (rxConected == 0)
        {
            crsf_devices[1].address = 0;
            strlcpy(crsf_devices[1].name, (const char *)"", CRSF_MAX_NAME_LEN);

            dbout.printf("no rx found\n");
        }
        else
        {
            if (crsf_devices[1].address == 0)
            {

                //CRSF_broadcast_ping();
            }
            else
            {
                if (rx_params_loaded < crsf_devices[1].number_of_params)
                {
                    dbout.printf("read rx info\n");
                    //next_param = 1;
                    //next_chunk = 0;
                    //CRSF_read_param(next_param, next_chunk, ELRS_RX_ADDRESS);
                }
            }
        }
    }
    else
    {
        crsf_devices[0].address = 0;
        strlcpy(crsf_devices[0].name, (const char *)"", CRSF_MAX_NAME_LEN);
        local_info.good_pkts = 0;
#if defined(debug)
        dbout.printf("no tx module found\n");
#endif
    }
    tickTime = currentMicros + tickInterval;
    rxConected = 0;
    txConected = 0;
}

const char *hdr_str_cb(const void *data)
{

    (void)data;
    //   dbout.printf("call params: %u: %i\n",count_params_loaded(), device_idx);

    if (count_params_loaded(device_idx) != crsf_devices[device_idx].number_of_params)
    {
        // dbout.printf("not all params: %u: %i\n",count_params_loaded(0), device_idx);

        snprintf(tempstring, sizeof tempstring, "%s %s", crsf_devices[device_idx].name, "LOADING");
    }
    else if (protocol_module_is_elrs())
    {
        dbout.printf("idx_elrs: %i\n", device_idx);

        snprintf(tempstring, sizeof tempstring, "%s  %d/%d  %c",
                 crsf_devices[device_idx].name, elrs_info.bad_pkts, elrs_info.good_pkts,
                 (elrs_info.flags & 1) ? 'C' : '-');
    }
    else
    {
        dbout.printf("tx module not detected\n");
        // return crsf_devices[device_idx].name;
        snprintf(tempstring, sizeof tempstring, "%s  %d/%d  %c",
                 crsf_devices[device_idx].name, elrs_info.bad_pkts, elrs_info.good_pkts,
                 (elrs_info.flags & 1) ? 'C' : '-');
    }
    return tempstring;
}

void bt_handle(uint8_t value)
{
    dbout.println("bt_handle");

    powerChangeHasRun = true;

    clickCurrentMicros = crsfTime + 500000; // 0.5sec
    dbout.printf("times: %u:%u\n", clickCurrentMicros / 1000, crsfTime / 1000);
    // powerChangeHasRun=true;

    // CRSF_read_param(crsfCmdPacket,1,next_chunk);
    // CRSF_write(crsfCmdPacket,8,0);

    // buildElrsPingPacket(crsfCmdPacket);
    // dbout.println(CRSF_send_model_id(2));

    // set modelId

    // turn on rx wifi, even if missmatch modelId
    // buildElrsPacket(crsfCmdPacket,16,1);

    CRSF_read_param(1, next_chunk, ELRS_ADDRESS);
    // serialEvent();
}
