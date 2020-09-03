add_executable(xglobe_wallpaper_changer wallpaper_changer.swift)
set_target_properties(xglobe_wallpaper_changer PROPERTIES Swift_LANGUAGE_VERSION 4.2)
install(TARGETS xglobe_wallpaper_changer
        DESTINATION bin
        COMPONENT exe)
