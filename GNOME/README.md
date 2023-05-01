### XRDP speedup
To speedup xrdp:
- Replace 32bit bpp with 16bit. File `/etc/xrdp/xrdp.ini`
```
max_bpp=16
```
- Disable GNOME animations
```
# run in gnome desktop terminal without sudo !!!
gsettings set org.gnome.desktop.interface enable-animations false

gsettings set org.gnome.shell.extensions.dash-to-dock animate-show-apps false
```
- Use black background wallpaper image - [wallpaper_black_20x20.png](wallpaper_black_20x20.png)

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
