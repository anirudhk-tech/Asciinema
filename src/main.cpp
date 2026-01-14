#include "asciinema/decoder.h"
#include "asciinema/frame.h"
#include <iostream>

int main(int argc, char* argv[]) {
    using namespace asciinema;
    
    // Check for video path argument
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <video_path>\n";
        std::cerr << "Example: " << argv[0] << " samples/test.mp4\n";
        return 1;
    }
    
    const std::string video_path = argv[1];
    
    // Create and open decoder
    VideoDecoder decoder;
    if (!decoder.open(video_path)) {
        std::cerr << "Error: Could not open video: " << video_path << "\n";
        return 1;
    }
    
    // Print video info
    std::cout << "Video loaded: " << video_path << "\n";
    std::cout << "  Resolution: " << decoder.width() << "x" << decoder.height() << "\n";
    std::cout << "  FPS: " << decoder.fps() << "\n";
    std::cout << "  Total frames: " << decoder.total_frames() << "\n";
    std::cout << "  Frame delay: " << decoder.frame_delay_ms() << " ms\n";
    std::cout << "\n";
    
    // Read first 10 frames 
    std::cout << "Reading first 10 frames...\n";
    int count = 0;
    while (auto frame = decoder.next_frame()) {
        std::cout << "  Frame " << frame->id 
                  << ": " << frame->image.cols << "x" << frame->image.rows
                  << " (" << (frame->image.total() * frame->image.elemSize()) << " bytes)\n";
        
        if (++count >= 10) break;
    }
    
    std::cout << "\nPhase 2 complete! Decoder working.\n";
    return 0;
}