//
// Created by bingo on 18-8-10.
//

#ifndef SERIAL_BK_PROTOCOL_H
#define SERIAL_BK_PROTOCOL_H


#define ARRSIZE(arr) (sizeof(arr)/sizeof(arr[0]))

#define len_bit (0x2)
#define seq_bit (0x3)
#define crc_bit (0x6)
#define user_data (0xff)

// MCU  TO ARM

const unsigned char mcu_cmd_mute[] = {0x55, 0xaa, 0x08, seq_bit, 0x0c, 0x01, 0x01, crc_bit};
#define mcu_cmd_mute_str "player cmd mut"

const unsigned char mcu_cmd_play_toggle[] = {0x55, 0xaa, 0x08, seq_bit, 0x0c, 0x01, 0x02, crc_bit};
#define mcu_cmd_play_toggle_str "player cmd play-pause"

const unsigned char mcu_cmd_pre[] = {0x55, 0xaa, 0x08, seq_bit, 0x0c, 0x01, 0x03, crc_bit};
#define mcu_cmd_pre_str "player cmd pre-song"

const unsigned char mcu_cmd_next[] = {0x55, 0xaa, 0x08, seq_bit, 0x0c, 0x01, 0x04, crc_bit};
#define mcu_cmd_next_str "player cmd next-song"

const unsigned char  mcu_cmd_forward_seek[] = { 0x55, 0xaa, 0x08, seq_bit, 0x0c, 0x01, 0x05, crc_bit };
#define mcu_cmd_forward_seek_str "player cmd forward-seek"

const unsigned char mcu_cmd_back_seek[] = {0x55, 0xaa, 0x08, seq_bit, 0x0c, 0x01, 0x06, crc_bit};
#define mcu_cmd_back_seek_str "player cmd back-seek"

const unsigned char mcu_cmd_play[] = {0x55, 0xaa, 0x08, seq_bit, 0x0c, 0x01, 0x07, crc_bit};
#define mcu_cmd_play_str "player cmd play"

const unsigned char mcu_cmd_pause[] = {0x55, 0xaa, 0x08, seq_bit, 0x0c, 0x01, 0x08, crc_bit};
#define mcu_cmd_pause_str "player cmd pause"

const unsigned char mcu_cmd_back_dir[] = {0x55, 0xaa, 0x08, seq_bit, 0x0c, 0x01, 0x09, crc_bit};
#define mcu_cmd_back_dir_str "player cmd back"

const unsigned char mcu_cmd_single_loop[] = {0x55, 0xaa, 0x08, seq_bit, 0x0c, 0x01, 0x0a, crc_bit};
#define mcu_cmd_single_loop_str "player cmd single-loop"

const unsigned char mcu_cmd_list_loop[] = {0x55, 0xaa, 0x08, seq_bit, 0x0c, 0x01, 0x0b, crc_bit};
#define mcu_cmd_list_loop_str "player cmd list-loop"

const unsigned char mcu_cmd_dir_loop[] = {0x55, 0xaa, 0x08, seq_bit, 0x0c, 0x01, 0x0c, crc_bit};
#define mcu_cmd_dir_loop_str "player cmd dir-loop"

const unsigned char mcu_cmd_play_spec[] = {0x55, 0xaa, 0x08, seq_bit, 0x0c, 0x01, 0x80, crc_bit};
#define mcu_cmd_play_spec_str "player cmd play-single"

const unsigned char mcu_cmd_dir_next[] = {0x55, 0xaa, 0x08, seq_bit, 0x0c, 0x01, 0x81, crc_bit};
#define mcu_cmd_dir_next_str "player cmd cd-next-dir"

const unsigned char mcu_info_play_pause[] = {0x55, 0xaa, 0x08, seq_bit, 0x0c, 0x02, 0x00, crc_bit};
#define mcu_info_play_pause_str "player info play-pause"

const unsigned char mcu_info_loop[] = {0x55, 0xaa, 0x08, seq_bit, 0x0c, 0x03, 0x00, crc_bit};
#define mcu_info_loop_str "player info loop"

const unsigned char mcu_info_play_time[] = {0x55, 0xaa, 0x08, seq_bit, 0x0c, 0x04, 0x00, crc_bit};
#define mcu_info_play_time_str "player info play-time"

const unsigned char mcu_info_play_total_time[] = {0x55, 0xaa, 0x08, seq_bit, 0x0c, 0x05, 0x00, crc_bit};
#define mcu_info_play_total_time_str "player info play-total-time"

const unsigned char mcu_info_index[] = {0x55, 0xaa, 0x08, seq_bit, 0x0c, 0x06, 0x00, crc_bit};
#define mcu_info_index_str "player info index"

const unsigned char mcu_info_total[] = {0x55, 0xaa, 0x08, seq_bit, 0x0c, 0x07, 0x00, crc_bit};
#define mcu_info_total_str "player info tatal-song"

const unsigned char mcu_info_name[] = {0x55, 0xaa, 0x08, seq_bit, 0x0c, 0x08, 0x00, crc_bit};
#define mcu_info_name_str "player info song-name"

const unsigned char mcu_info_singer[] = {0x55, 0xaa, 0x08, seq_bit, 0x0c, 0x09, 0x00, crc_bit};
#define mcu_info_singer_str "player info singer"

const unsigned char mcu_info_ablume[] = {0x55, 0xaa, 0x08, seq_bit, 0x0c, 0x0a, 0x00, crc_bit};
#define mcu_info_ablume_str "player info album"

const unsigned char mcu_info_file_name[] = {0x55, 0xaa, 0x08, seq_bit, 0x0c, 0x0b, 0x00, crc_bit};
#define mcu_info_file_name_str "player info file-names"

/* #ARM TO MCU */
const unsigned char arm_feedback_play_pause[] = { 0xaa, 0x55, 0x08, seq_bit, 0x0c,   0x02, 0x00, crc_bit};
#define arm_feedback_play_pause_str "player play-pause"

const unsigned char arm_feedback_loop[] = { 0xaa, 0x55, 0x08, seq_bit, 0x0c,   0x03, 0x00, crc_bit};
#define arm_feedback_loop_str "player loop"

const unsigned char arm_feedback_time_play[] = {0xaa, 0x55, 0x08, seq_bit, 0x0c, 0x04, 0x00, crc_bit};
#define arm_feedback_time_play_str "player play-time"

const unsigned char arm_feedback_time_song[] = {0xaa, 0x55, 0x08, seq_bit, 0x0c, 0x05, 0x00, crc_bit};
#define arm_feedback_time_song_str "player play-total-time"

const unsigned char arm_feedback_cur[] = {0xaa, 0x55, 0x08, seq_bit, 0x0c, 0x06, 0x00, crc_bit};
#define arm_feedback_cur_str "player cur"

const unsigned char arm_feedback_total_song[] = {0xaa, 0x55, 0x08, seq_bit, 0x0c, 0x07, 0x00, crc_bit};
#define arm_feedback_total_song_str "player tatal-song"

const unsigned char  arm_feedback_name[] =  { 0xaa, 0x55, 0x08, seq_bit, 0x0c, 0x08, 0x00, crc_bit };
#define arm_feedback_name_str "player song-name"

const unsigned char arm_feedback_singer[] = {0xaa, 0x55, 0x08, seq_bit, 0x0c, 0x09, 0x00, crc_bit};
#define arm_feedback_singer_str "player singer"

const unsigned char arm_feedback_ablum[] = {0xaa, 0x55, 0x08, seq_bit, 0x0c, 0x0a, 0x00, crc_bit};
#define arm_feedback_ablum_str "player album"

const unsigned char arm_feedback_file_names[] = {0xaa, 0x55, 0x08, seq_bit, 0x0c, 0x0b, 0x00, crc_bit};
#define arm_feedback_file_names_str "player file-names"

const unsigned char arm_send_player_status[] = {0xaa, 0x55, 0x08, seq_bit, 0x0c, 0x02, user_data, crc_bit};
#define arm_send_player_status_str "player status"  // play or pause

//typedef message *  (*a2mfunc)(const char * , message * msg);


#endif //SERIAL_BK_PROTOCOL_H
