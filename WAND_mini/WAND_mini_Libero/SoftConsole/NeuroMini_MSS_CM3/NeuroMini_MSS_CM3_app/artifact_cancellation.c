///* pseudo code for doing double buffering + artifact cancellation
//
//each time we need to read 64 times from each fifo, but only store 100 of 128 channels, in a row, in adc_data_buf1 or adc_data_buf2
//
//MAY NOT WORK IF THERE ARE CONSECUTIVE SAMPLES WITH STIM_FLAG = 1
//ALSO WILL NOT WORK IF ENABLED CHANNELS ARE CHANGED
//
// */
//
//static uint16_t adc_data_buf1[100];
//static uint16_t adc_data_buf2[100];
//static uint16_t sel_data_vector[128];
//
//bool artifact_en = 0; // whether we should do artifact cancellation or not
//bool artifact_flag = 0; // whether we need to do artifact cancellation this sample or not
//int chan = 0;
//bool stim_flag = 0;
//int dummy = 0;
//bool sample_bool = True; // True is populate adc_data_buf1 send adc_data_buf2, false is populate adc_data_buf2 send adc_data_adc_data_buf1.
//
//while(1)
//{
//    if ((*status & NM0_DATA) && (*status & NM1_DATA)) {
//        chan = 0;
//        if (artifact_en && stim_flag) // doing artifact cancellation and there was stim last time
//            artifact_flag = 1; // set flag that this time we should do artifact cancellation (i.e. avg of adc_data_buf1 and adc_data_buf2)
//        else
//            sample_bool = ! sample_bool; // proceed as usual with double buffering
//        stim_flag = 0;
//        // read NM0 adc data
//        for(i = 0; i < 66; i++)
//        {
//            if (i == 0)
//            {
//                flags = *n0adc;
//            }
//            else if (i == 65)
//            {
//                crcflag0 = *n0adc;
//            }
//            else
//            {
//                if (sel_data_vector[i-1] == 0)
//                {
//                    dummy = *n0adc;
//                }
//                else
//                {
//                    if (artifact_flag)
//                        sample_bool ? adc_data_buf1[chan+1] = (*n0adc>>1) + (adc_data_buf2[chan+1]>>1) : adc_data_buf2[chan+1] = (*n0adc>>1) + (adc_data_buf1[chan+1]>>1);
//                    else
//                        sample_bool ? adc_data_buf1[chan+1] = *n0adc : adc_data_buf2[chan+1] = *n0adc;
//                    chan++;
//                }
//                // check if current sample had any stim
//                if (sample_bool)
//                {
//                    if (artifact_en && !stim_flag && (adc_data_buf1[chan+1] & 0x8000))
//                        stim_flag = 1;
//                }
//                else
//                {
//                    if (artifact_en && !stim_flag && (adc_data_buf2[chan+1] & 0x8000))
//                        stim_flag = 1;
//                }
//            }
//        }
//
//        for(i = 0; i < 66; i++)
//        {
//            if (i == 0)
//            {
//                flags = *n1adc;
//            }
//            else if (i == 65)
//            {
//                crcflag1 = *n1adc;
//            }
//            else
//            {
//                if (sel_data_vector[i+63] == 0)
//                {
//                    dummy = *n1adc;
//                }
//                else
//                {
//                    if (artifact_flag)
//                        sample_bool ? adc_data_buf1[chan+1] = (*n1adc>>1) + (adc_data_buf2[chan+1]>>1) : adc_data_buf2[chan+1] = (*n1adc>>1) + (adc_data_buf1[chan+1]>>1);
//                    else
//                        sample_bool ? adc_data_buf1[chan+1] = *n1adc : adc_data_buf2[chan+1] = *n1adc;
//                    chan++;
//                }
//                // check if current sample had any stim
//                if (sample_bool)
//                {
//                    if (artifact_en && !stim_flag && (adc_data_buf1[chan+1] & 0x8000))
//                        stim_flag = 1;
//                }
//                else
//                {
//                    if (artifact_en && !stim_flag && (adc_data_buf2[chan+1] & 0x8000))
//                        stim_flag = 1;
//                }
//            }
//        }
//
//        artifact_flag = 0;
//        if sample_bool && !stim_flag
//            // send adc_data_buf2
//        if !sample_bool && !stim_flag
//            // send adc_data_buf1
//    }
//
//}
