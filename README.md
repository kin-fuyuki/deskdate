# DESKDATE - cascading application menu generator
<img width="1280" height="720" alt="image" src="https://github.com/user-attachments/assets/ea60c6ea-5b71-4acc-893d-5629becd5ac1" />

## requirements
xfce4
tiny

## INSTALLATION
(i recommend you having a folder like ~/src/libs to ynow.. dont make a mess with git clones. thats my tip as a linux user that been using this system for so long. but if you have a place where to save gits, then use it)

### install tiny if you dont have it already
```
https://github.com/kin-fuyuki/tiny-cpp.git
cd tiny-cpp
sudo mkdir /usr/local/include/tiny
sudo cp inc/* /usr/local/include/tiny/
cd ..
```

```
git clone https://github.com/kin-fuyuki/deskdate.git
cd deskdate
cmake .
make
sudo cp deskdate /usr/local/bin/
cd ..
```

## USAGE

### first steps
1) create a folder at ~/.config/deskdate
2) add at least one .desktop or directory (and subdirs.. all depends on what you want) with a .desktop there
3) run `deskdate ~/.config/deskdate ~/.config/startmenulist.xml`
4) run `touch ~/.config/deskdate/R` to flag deskdate to update the xml
5) right click xfce's applications menu, click properties, and at the bottom click `use custom menu file:` and the menu file button. then use the ~/.config/startmenulist.xml file generated

### sorting
inside each folder you can create a sort.txt file to sort the icons, just write the file NAME without extension in the order you want
