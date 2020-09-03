import Foundation
import AppKit

// Simple wallpaper changer
for argument in CommandLine.arguments {
    do {
        let imgurl = NSURL.fileURL(withPath: argument)
        if try imgurl.checkResourceIsReachable() == false {
            exit(-1)
        }
        let workspace = NSWorkspace.shared
        if let screen = NSScreen.main  {
            try workspace.setDesktopImageURL(imgurl, for: screen, options: [:])
        }
    } catch {
        print(error)
        exit(-1)
    }
}
