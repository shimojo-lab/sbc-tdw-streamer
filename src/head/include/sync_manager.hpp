/*************************************************
*                sync_manager.hpp                *
*  (the manager of the synchronization process)  *
*************************************************/

#ifndef SYNC_MANAGER_HPP
#define SYNC_MANAGER_HPP

#include "mutex_logger.hpp"
#include "socket_utils.hpp"
#include "sync_utils.hpp"
#include "json_handler.hpp"
#include <cmath>
extern "C"{
    #include <turbojpeg.h>
}

using streambuf_ptr_t = std::shared_ptr<_asio::streambuf>;

/* the manager of the synchronization process */
class SyncManager{
    private:
        _asio::io_service& ios;                    // the I/O event loop
        std::vector<sock_ptr_t>& socks;            // in-use TCP sockets
        std::vector<streambuf_ptr_t> stream_bufs;  // the stream buffers
        const int display_num;                     // the number of the displays
        const int target_fps;                      // the target frame rate
        std::atomic_int sync_count;                // the count of the synchronized displays
        jpeg_params_t& ycbcr_format_list;          // the YCbCr formats applied for the display nodes
        jpeg_params_t& quality_list;               // the quality factors applied for the display nodes
        JsonHandler sync_params;                   // a parsed sync message
        hr_clock_t pre_t;                          // the starting time of a term
        int frame_count = 0;                       // the count of obsoleted frames
        
        const std::string changeYCbCr(const int change_flag, const int id);    // change the YCbCr format
        const std::string changeQuality(const int change_flag, const int id);  // change the quality factor
        void parseSyncMsg(const std::string& msg, const int id);               // parse a sync message
        void onRecvSync(const err_t& err, size_t t_bytes, const int id);       // the callback when receiving a sync message
        void onSendSync(const err_t& err, size_t t_bytes, const int id);       // the callback when sending a sync message
        void sendSync();                                                       // send a sync message
        
    public:
        SyncManager(_asio::io_service& ios, std::vector<sock_ptr_t>& socks,  // constructor
                    const int target_fps, jpeg_params_t& ycbcr_format_list,
                    jpeg_params_t& quality_list);
        void run();  // start the synchronizaton process
};

#endif  /* SYNC_MANAGER_HPP */

