#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <stdlib.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <boost/asio.hpp>

/* 名前空間のエイリアス */
namespace pt = boost::property_tree;
namespace asio = boost::asio;

/* 別スレッドでフレーム送信を行う関数 */
void sendTrimmedFrame(int id, cv::Mat frame){
    std::vector<uchar> buf(frame.rows*frame.cols*frame.channels());
    std::vector<int> params = {cv::IMWRITE_JPEG_QUALITY, 95};
    cv::imencode(".jpg", frame, buf, params);    
}

/* 動画フレーム分割器 */
class VideoDemuxer{
    private:
        cv::VideoCapture video;  // 再生する動画
        cv::Rect *rects;         // フレームの分割領域
        cv::Mat *div_frames;     // 分割フレーム
        int x;                   // ディスプレイの横の枚数
        int y;                   // ディスプレイの縦の枚数
        int frame_num = 0;       // 現在のフレーム番号
        int total_frame_num;     // 総フレーム数
        double fps;              // 動画のフレームレート
    public:
        VideoDemuxer(const char *filename, int x, int y);  // コンストラクタ
        ~VideoDemuxer();                                   // デストラクタ
        bool loadVideo(const char *filename);              // 動画ファイルを読み込み
        void divideNextFrame();                            // 次番のフレームを分割
        cv::Mat *getDividedFrame(int x, int y);            // 分割フレームを取得
};

// コンストラクタ
VideoDemuxer::VideoDemuxer(const char *filename, int x, int y){
    // パラメータを設定
    this->x = x;
    this->y = y;
    
    // 動画ファイルを読み込み
    if(this->loadVideo(filename) == false){
        exit(-1);
    }
}

// デストラクタ
VideoDemuxer::~VideoDemuxer(){
    delete[] this->rects;
    delete[] this->div_frames;
}

// 動画ファイルを読み込むメソッド
bool VideoDemuxer::loadVideo(const char *filename){
    // 動画ファイルを読み込み
    cv::VideoCapture video(filename);
    if(video.isOpened()){
        this->video = video;
    }else{
        std::cerr << "[Error] Could not open file." << std::endl;
        return false;
    }
    
    // 動画ファイルの情報を取得
    int width = int(video.get(CV_CAP_PROP_FRAME_WIDTH)/this->x);
    int height = int(video.get(CV_CAP_PROP_FRAME_HEIGHT)/this->y);
    this->total_frame_num = video.get(CV_CAP_PROP_FRAME_COUNT);
    this->fps = video.get(CV_CAP_PROP_FPS);
    
    // フレームの分割方法を決定
    int display_num = x * y;
    this->rects = new cv::Rect[display_num];
    this->div_frames = new cv::Mat[display_num];
    for(int j=0; j<this->y; ++j){
        for(int i=0; i<this->x; ++i){
            this->rects[i+this->x*j] = cv::Rect(i*width, j*height, width, height);
        }
    }
    return true;
}

// 次番のフレームを分割するメソッド
void VideoDemuxer::divideNextFrame(){
    // 動画からフレームを取得
    cv::Mat frame;
    this->video >> frame;
    
    // フレームを分割
    for(int j=0; j<this->y; ++j){
        for(int i=0; i<this->x; ++i){
            int idx = i + this->x * j;
            this->div_frames[idx] = cv::Mat(frame, this->rects[idx]);
        }
    }
    return;
}

// 分割フレームを取得するメソッド
cv::Mat *VideoDemuxer::getDividedFrame(int x, int y){
    return &(this->div_frames[x*this->y*y]);
}

/* Main関数 */
int main(int argc, char* argv[]){
    // 設定ファイルをパース
    pt::ptree conf;
    pt::read_json("config.json", conf);
    boost::optional<std::string> filename = conf.get_optional<std::string>("head_node.filename");
    boost::optional<int> x = conf.get_optional<int>("head_node.layout.x");
    boost::optional<int> y = conf.get_optional<int>("head_node.layout.y");
    boost::optional<int> width = conf.get_optional<int>("head_node.resolution.width");
    boost::optional<int> height = conf.get_optional<int>("head_node.resoution.height");
    pt::ptree display_node = conf.get_child("display_node");
    
    // 動画フレーム分割器を起動
    VideoDemuxer demuxer(filename->c_str(), *x, *y);
    demuxer.divideNextFrame();
    
    // フレーム送信スレッドを起動
    std::vector<std::thread> threads;
    return 0;
}

