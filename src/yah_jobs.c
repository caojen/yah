#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <zlib.h>
#include <errno.h>
#include <sys/wait.h>

#include "yah_core.h"
#include "yah_log.h"
#include "yah_exec.h"
#include "yah_airodump.h"
#include "yah_pty.h"
#include "yah_const.h"
#include "yah_thread_pool.h"
#include "yah_config.h"
#include "yah_mem.h"
#include "yah_string.h"
#include "yah_lru.h"
#include "yah_sqlite3.h"
#include "yah_json.h"
#include "yah_base64.h"
#include "yah_http.h"
#include "cjson.h"

void
yah_rp_pool_job_func_ap(struct yah_airodump_data* data) {}

void 
yah_rp_pool_job_func_apstation(struct yah_airodump_data* data) {}

// void
// yah_rp_pool_job_func_ap(struct yah_airodump_data* data) {
//     // this function is for ap, pushing ap to remote address
//     // generate data into json
//     time_t now = time(NULL);

//     yah_log("yah_rp_ap started");

//     cjson_object* json = cjson_object_init();
//     cjson_string* cjson_string_id = cjson_string_parse("id");
//     cjson_string* cjson_string_mobile = cjson_string_parse("mobile");
//     cjson_string* cjson_string_bssid = cjson_string_parse("bssid");
//     cjson_string* cjson_string_comment = cjson_string_parse("comment");
//     cjson_string* cjson_string_createtime = cjson_string_parse("create_time");
//     cjson_string* cjson_string_etltime = cjson_string_parse("etl_time");

//     cjson_number* cjson_value_id = cjson_number_integer(data->data.ap.id);
//     cjson_number* cjson_value_mobile = cjson_number_integer(device_number);
//     cjson_string* cjson_value_bssid = cjson_string_parse(data->data.ap.bssid);
//     cjson_string* cjson_value_comment = cjson_string_parse(data->data.ap.comment);
//     char create_time[32];
//     char etl_time[32];
//     YAH_JSON_PARSE_TIME(data->data.ap.create_time, create_time);
//     YAH_JSON_PARSE_TIME(now, etl_time);
//     cjson_string* cjson_value_createtime = cjson_string_parse(create_time);
//     cjson_string* cjson_value_etltime = cjson_string_parse(etl_time);

//     cjson_object_set(json, cjson_string_id, cjson_value_id);
//     cjson_object_set(json, cjson_string_mobile, cjson_value_mobile);
//     cjson_object_set(json, cjson_string_bssid, cjson_value_bssid);
//     cjson_object_set(json, cjson_string_comment, cjson_value_comment);
//     cjson_object_set(json, cjson_string_createtime, cjson_value_createtime);
//     cjson_object_set(json, cjson_string_etltime, cjson_value_etltime);

//     cjson_array* origin = cjson_array_init();
//     cjson_array_push(origin, json);

//     // serialize the json
//     char str[YAH_JSON_SINGAL_MAX] = { 0 };
//     cjson_array_stringify(origin, str, YAH_JSON_SINGAL_MAX);
//     yah_log("cjson parse result: %s", str);

//     cjson_array_free_whole(origin);

//     // gzip compress
//     unsigned char dest[YAH_JSON_SINGAL_COMPRESS_MAX];
//     unsigned int bufsize = YAH_JSON_SINGAL_COMPRESS_MAX;
//     z_stream strm;
//     strm.zalloc = Z_NULL;
//     strm.zfree  = Z_NULL;
//     strm.opaque = Z_NULL;
//     strm.avail_in = strlen(str) + 1;
//     strm.avail_out = bufsize;
//     strm.next_in = (Bytef *)str;
//     strm.next_out = (Bytef *)dest;
//     deflateInit2(&strm, Z_DEFAULT_COMPRESSION, Z_DEFLATED, MAX_WBITS+16, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY);
//     deflate(&strm, Z_FINISH);
//     bufsize = bufsize - strm.avail_out;
//     deflateEnd(&strm);

//     // convert into base64
//     char* final = base64_encode(dest, bufsize);

//     // generate the final data
//     cjson_object* body = cjson_object_init();
//     cjson_string* cjson_string_data = cjson_string_parse("data");
//     cjson_string* cjson_value_data = cjson_string_parse(final);
//     cjson_object_set(body, cjson_string_data, cjson_value_data);
//     char body_str[YAH_JSON_SINGAL_MAX] = { 0 };
//     cjson_object_stringify(body, body_str, YAH_JSON_SINGAL_MAX);
//     yah_log("body generated: %s", body_str);

//     cjson_object_free_whole(body);
//     free(final);

//     // generate the socket
//     Request* request = yah_request_init();
//     char __remote_ip[20] = { 0 };
//     yah_get_remote_local(__remote_ip);
//     if(__remote_ip[0] == 0) {
//         yah_log("dns failed, skip this job, sleep 2 and goto err push back");
//         sleep(2);
//         goto errpushback;
//     }
//     yah_request_set_remote(request, __remote_ip, remote_port);
//     yah_request_set_method(request, REQUEST_METHOD_POST);
//     yah_request_set_url(request, YAH_REMOTE_URL_AP);
//     yah_request_add_header(request, "Content-Type", "application/json;charset=UTF-8");
//     yah_request_add_header(request, "Host", YAH_REMOTE_DESC);
//     yah_request_add_header(request, "User-Agent", "Mozilla/5.0 (Linux; Android 6.0; Nexus 5 Build/MRA58N) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/90.0.4430.93 Mobile Safari/537.36");
//     yah_request_set_body(request, body_str);
//     int code = yah_request_send(request);
//     if(code != REQUEST_OK) {
//         yah_log("sending ap return %d, pushing back for ap's id = %d", code, data->data.ap.id);
//         goto errpushback;
//     }
//     yah_request_destory(request);

//     // set data is_uploaded in database
//     yah_airodump_data_updated(data);
//     yah_log("yah_rp_ap: job ended");
//     return;
// errpushback:    ;
//     // data is not uploaded, push it back to rp_pool's queue
//     struct yah_job* job = YAH_JOB_INITIALIZER;
//     struct yah_airodump_data* newdata = (struct yah_airodump_data*)
//         yah_mem_alloc(sizeof(struct yah_airodump_data));
//     memcpy(newdata, data, sizeof(struct yah_airodump_data));
//     job->arg = (void*) newdata;
//     job->arg_destory = yah_mem_free;
//     job->func = yah_rp_pool_job_func;
//     yah_thread_pool_push_job(rp_pool, job);
//     yah_log("yah_rp_ap: errpushback: %p", data);
// }

// void
// yah_rp_pool_job_func_apstation(struct yah_airodump_data* data) {
//     // this function is for apstation, pushing apstation to remote address
//     // generate data into json
//     time_t now = time(NULL);

//     yah_log("yah_rp_apstation started");

//     cjson_object* json = cjson_object_init();
//     cjson_string* cjson_string_id = cjson_string_parse("id");
//     cjson_string* cjson_string_mobile = cjson_string_parse("mobile");
//     cjson_string* cjson_string_bssid = cjson_string_parse("bssid");
//     cjson_string* cjson_string_station = cjson_string_parse("station");
//     cjson_string* cjson_string_comment = cjson_string_parse("comment");
//     cjson_string* cjson_string_createtime = cjson_string_parse("create_time");
//     cjson_string* cjson_string_etltime = cjson_string_parse("etl_time");

//     cjson_number* cjson_value_id = cjson_number_integer(data->data.apstation.id);
//     cjson_number* cjson_value_mobile = cjson_number_integer(device_number);
//     cjson_string* cjson_value_bssid = cjson_string_parse(data->data.apstation.bssid);
//     cjson_string* cjson_value_station = cjson_string_parse(data->data.apstation.station);
//     cjson_string* cjson_value_comment = cjson_string_parse(data->data.apstation.comment);
//     char create_time[32];
//     char etl_time[32];
//     YAH_JSON_PARSE_TIME(data->data.apstation.create_time, create_time);
//     YAH_JSON_PARSE_TIME(now, etl_time);
//     cjson_string* cjson_value_createtime = cjson_string_parse(create_time);
//     cjson_string* cjson_value_etltime = cjson_string_parse(etl_time);

//     cjson_object_set(json, cjson_string_id, cjson_value_id);
//     cjson_object_set(json, cjson_string_mobile, cjson_value_mobile);
//     cjson_object_set(json, cjson_string_bssid, cjson_value_bssid);
//     cjson_object_set(json, cjson_string_comment, cjson_value_comment);
//     cjson_object_set(json, cjson_string_createtime, cjson_value_createtime);
//     cjson_object_set(json, cjson_string_etltime, cjson_value_etltime);
//     cjson_object_set(json, cjson_string_station, cjson_value_station);

//     cjson_array* origin = cjson_array_init();
//     cjson_array_push(origin, json);

//     // serialize the json
//     char str[YAH_JSON_SINGAL_MAX] = { 0 };
//     cjson_array_stringify(origin, str, YAH_JSON_SINGAL_MAX);
//     yah_log("cjson parse result: %s", str);

//     // gzip compress
//     unsigned char dest[YAH_JSON_SINGAL_COMPRESS_MAX];
//     unsigned int bufsize = YAH_JSON_SINGAL_COMPRESS_MAX;

//     // z_stream strm;
//     // compress(dest, &bufsize, str, strlen(str) + 1);
//     z_stream strm;
//     strm.zalloc = Z_NULL;
//     strm.zfree  = Z_NULL;
//     strm.opaque = Z_NULL;
//     strm.avail_in = strlen(str) + 1;
//     strm.avail_out = bufsize;
//     strm.next_in = (Bytef *)str;
//     strm.next_out = (Bytef *)dest;
//     deflateInit2(&strm, Z_DEFAULT_COMPRESSION, Z_DEFLATED, MAX_WBITS+16, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY);
//     deflate(&strm, Z_FINISH);
//     bufsize = bufsize - strm.avail_out;
//     deflateEnd(&strm);

//     cjson_array_free_whole(origin);
    
//     // convert into base64 code
//     char* final = base64_encode(dest, bufsize);
    
//     // generate the final data
//     cjson_object* body = cjson_object_init();
//     cjson_string* cjson_string_data = cjson_string_parse("data");
//     cjson_string* cjson_value_data = cjson_string_parse(final);
//     cjson_object_set(body, cjson_string_data, cjson_value_data);
//     char body_str[1024] = { 0 };
//     cjson_object_stringify(body, body_str, 1024);
//     yah_log("body generated: %s", body_str);

//     cjson_object_free_whole(body);

//     free(final);

//     // generate the socket
//     Request* request = yah_request_init();
//     char __remote_ip[20] = { 0 };
//     yah_get_remote_local(__remote_ip);
//     if(__remote_ip[0] == 0) {
//         yah_log("dns failed, skip this job, sleep 2 and goto err push back");
//         sleep(2);
//         goto errpushback;
//     }
//     yah_request_set_remote(request, __remote_ip, remote_port);
//     yah_request_set_method(request, REQUEST_METHOD_POST);
//     yah_request_set_url(request, YAH_REMOTE_URL_APSTATION);
//     yah_request_add_header(request, "Content-Type", "application/json;charset=UTF-8");
//     yah_request_add_header(request, "Host", YAH_REMOTE_DESC);
//     yah_request_add_header(request, "User-Agent", "Mozilla/5.0 (Linux; Android 6.0; Nexus 5 Build/MRA58N) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/90.0.4430.93 Mobile Safari/537.36");
//     yah_request_set_body(request, body_str);
//     int code = yah_request_send(request);
//     if(code != REQUEST_OK) {
//         yah_log("sending apstation return %d, pushing back for apstation's id = %d", code, data->data.apstation.id);
//         goto errpushback;
//     } else {
//         // set data is uploaded in database
//         yah_airodump_data_updated(data);
//     }
//     yah_request_destory(request);
//     yah_log("yah_rp_apstation: job ended");
//     return;

// errpushback:    ;
//     // data is not uploaded, push it back to rp_pool's queue
//     struct yah_job* job = YAH_JOB_INITIALIZER;
//     struct yah_airodump_data* newdata = (struct yah_airodump_data*)
//         yah_mem_alloc(sizeof(struct yah_airodump_data));
//     memcpy(newdata, data, sizeof(struct yah_airodump_data));
//     job->arg = (void*) newdata;
//     job->arg_destory = yah_mem_free;
//     job->func = yah_rp_pool_job_func;
//     yah_thread_pool_push_job(rp_pool, job);
//     yah_log("yah_rp_apstation: errpushback: %p", data);
// }
