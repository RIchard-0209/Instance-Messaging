<<<<<<< HEAD
# IM Server

=======
# IM





## 单例类



## 网络

需要在`.pro`文件中添加网络模块![image-20250217164214054](https://gitee.com/R1cha2dli/pic-bed/raw/master/img/image-20250217164214054.png)

![image-20250217164420795](https://gitee.com/R1cha2dli/pic-bed/raw/master/img/image-20250217164420795.png)

继承一个以自己为模板的模板基类，是CRTP技术。只有在运行的时候会实例化。



>>>>>>> IM-Client





QT的ui设计，按tab的时候会按照设置的顺序切换。

- 切换到设计模式，点击“编辑TAB顺序”
  ![在这里插入图片描述](https://i-blog.csdnimg.cn/blog_migrate/5010ccd4d70a2b4ea227c7d22d60703a.png)
- 鼠标点点点，指定TAB顺序
  ![在这里插入图片描述](https://gitee.com/R1cha2dli/pic-bed/raw/master/img/3169ee5c11720824a9c74fe6b8c3eceb.png)



```cpp
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    /*
    ui->lineEdit_username;
    ui->lineEdit_password;
    ui->checkBox_remember;
    ui->pushButton_login;
    */
    
    setTabOrder(ui->lineEdit_username, ui->checkBox_remember);
    setTabOrder(ui->checkBox_remember, ui->lineEdit_password);
    setTabOrder(ui->lineEdit_password, ui->pushButton_login);
    //setTabOrder(ui->pushButton_login, ui->lineEdit_username);//not need.

    //禁止CheckBox响应TAB按键
    ui->checkBox_remember->setFocusPolicy(Qt::NoFocus);
}

```

