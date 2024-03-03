# Ubuntu20.04安装小程序环境

## 使用环境

==python2.7== ^[1]
(Ubuntu only have python3, but you can install python2 and use it)

安装：

```sh
sudo apt install python2
```

## 权限设置^[2]

```sh
cd /usr/bin
```

查看所有关于python的相关信息;

```sh
ls -al *python*
```

更改权限(前提是输入python2/python3后显示无'command',如果可用,忽略此步骤)

```sh
sudo chgrp -R [username] python2
sudo chgrp -R [username] python3
```

## 安装qt4^[3]

Ubuntu20.04已经移除了对Qt库的支持，无法使用

```sh
sudo apt-get install python-pyqt4
```



在终端输入：

```sh
sudo add-apt-repository ppa:rock-core/qt4
sudo apt-get update
```

再输入：

```sh
cd ~/Downloads
wget http://archive.ubuntu.com/ubuntu/pool/universe/q/qt-assistant-compat/libqtassistantclient4_4.6.3-7build1_amd64.deb
sudo apt-get install ./libqtassistantclient4_4.6.3-7build1_amd64.deb
wget http://archive.ubuntu.com/ubuntu/pool/universe/p/python-qt4/python-qt4_4.12.1+dfsg-2_amd64.deb
sudo apt-get install ./python-qt4_4.12.1+dfsg-2_amd64.deb
wget http://archive.ubuntu.com/ubuntu/pool/universe/p/python-pyaudio/python-pyaudio_0.2.11-1build2_amd64.deb
sudo apt-get install ./python-pyaudio_0.2.11-1build2_amd64.deb
```
若出现` Do you want to continue? [Y/n] y Abort.`
请在sudo apt-install 指令后加-y

Such as:
```sh
cd ~/Downloads
wget http://archive.ubuntu.com/ubuntu/pool/universe/q/qt-assistant-compat/libqtassistantclient4_4.6.3-7build1_amd64.deb
sudo apt-get install ./libqtassistantclient4_4.6.3-7build1_amd64.deb -y
wget http://archive.ubuntu.com/ubuntu/pool/universe/p/python-qt4/python-qt4_4.12.1+dfsg-2_amd64.deb
sudo apt-get install ./python-qt4_4.12.1+dfsg-2_amd64.deb -y
wget http://archive.ubuntu.com/ubuntu/pool/universe/p/python-pyaudio/python-pyaudio_0.2.11-1build2_amd64.deb
sudo apt-get install ./python-pyaudio_0.2.11-1build2_amd64.deb -y

```


## 运行小程序

```sh
python2 irsim.pyc
```



## Notice

本人在安装的过程中，先在无python2的环境下安装了qt环境，导致安装完python2后报错

```python
No module named PyQt4.QtCore
```

然后又在pyc所在文件夹里重新安装了一遍，就可以了；

花了接近一小时的时间去搞小程序的环境，伤不起~QAQ

## Reference
[1] 我和hygg的聊天记录，得知需要python2.7
[2] [修改执行权限](https://blog.csdn.net/weixin_44214775/article/details/117885285)
[3] [关于在Ubuntu20.04中安装python-pyqt4的问题](https://blog.csdn.net/lin88556/article/details/124425666)
