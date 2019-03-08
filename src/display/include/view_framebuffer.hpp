/****************************************
*          view_framebuffer.hpp         *
*  (the framebuffer to put raw frames)  *
****************************************/

#ifndef VIEW_FRAMEBUFFER_HPP
#define VIEW_FRAMEBUFFER_HPP

#include "sync_utils.hpp"
#include <memory>
#include <thread>

const int COLOR_CHANNEL_NUM = 3;          // the number of the color channels
const int VIEWBUF_SPINLOCK_INTERVAL = 1;  // the interval in the spinlock

/* the framebuffer to put raw frames */
class ViewFramebuffer{
    private:
        const int page_num;                         // the number of domains in the buffer
        std::vector<unsigned char*> page_ptrs;      // the pointers of domains in the buffer
        std::vector<std::atomic_bool> page_states;  // the flag to switch the state of each domain
        int cur_page = 0;                           // the domain on which the next frame is put
    
    public:
        ViewFramebuffer(const int width, const int height,  // constructor
                        const int page_num);
        ~ViewFramebuffer();                                 // destructor
        unsigned char *getDrawPage(const int id);           // get a domain to put a new frame
        const unsigned char *getDisplayPage();              // 
        const int getCurrentPage();                         // get the value of cur_page
        void activatePage(const int id);                    // make a domain available
        void deactivatePage();                              // make a domain unavailable
};

using viewbuf_ptr_t = std::shared_ptr<ViewFramebuffer>;

#endif  /* VIEW_FRAMEBUFFER_HPP */

