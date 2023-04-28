### GNOME App Launcher
To add App Launcher create file `~/.local/share/applications/<app_name>.desktop`
```bash
[Desktop Entry]
Encoding=UTF-8
Version=1.0
Type=Application
Terminal=false
Exec=/home/ec2-user/bin/Netron-6.8.5.AppImage
Name=Netron
Icon=netron
```

Put icon file (png, svg) e.g. `netron.svg` to `~/.local/share/icons`.
