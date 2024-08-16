# Chinese introduction DUIThink
官网：http://121.5.53.246/
QQ群：885314879
视频教程：https://space.bilibili.com/322458488?spm_id_from=333.788.b_765f7570696e666f.1

初衷：
       mfc做不了绚丽的ui，很多地方需要自绘，修改起来也麻烦
        一些ui库手写xml效率低下，预览调整也麻烦，非所见即所得，一些控件属性记不住还要对照属性表使用

DUIThink库：
        1、控件布局靠上、靠下、靠左、靠右、居中、相对、绝对
        2、控件大小固定宽高、最大最小宽高、根据文本自动调整宽高、根据子控件自动调整宽高
        3、控件背景颜色、图片、圆角、渐变颜色
        4、支持svg图片，比浏览器支持的格式多（很多软件都不完美支持，例如wps看图、一些开源的svg库等）
        5、支持dpi，dpi切换只需要一行代码
        6、控件Clone接口，可以复制出一模一样的view，如果你需要重复的view，那么它适合你
        7、控件Attach、Dettach接口，控件可以随意挂载父节点
        8、RichText绘制，一条文本可以分不同的字体、不同颜色渲染，且支持多行、靠左、靠右、靠上、靠下、居中
        9、自绘Edit控件，支持插入图片、动图、文字透明通道等，可做聊天消息编辑框
        10、自绘ip控件
        11、自绘日历控件
        12、3d旋转菜单控件

        13、Clock时钟控件

        14、Combox控件，支持梯度子项滑动，类似以前QQ登录选择账号时的特效

        15、Animate控件，支持Gif、序列帧

        16、ListView控件，修改一个属性即可实现List、Grid、TileH、TileV模式之间切换，支持子项间随意拖动交换位置

        17、树控件，带选中按钮树、无选中按钮树、带表头树、单选树修改属性即可完成切换

        18、丰富的窗体特效

        19、控件水波纹特效

        20、一键换肤功能，只需要一行代码

        21、控件自定义Bitmap背景

 源码：GitHub - miaomiaoyule/DUIThink

DTDesigner设计器：
        1、像mfc一样自动生成窗体类、控件变量、控件事件代码

        2、任意拖放、对齐控件，修改控件属性所见即所得

        3、图片、字体、颜色资源一览无余

        4、控件树视图Shift+Up\Down实时调整控件Z序

        5、实时进行菜单一级、二级、三级...编辑

        6、支持Ctrl+C、Ctrl+V、Ctrl+Z、Ctrl+Y

        7、复制ui到另一个视图里，无需重复制作ui

        8、支持开发者自定义控件

        9、实时预览窗体各dpi效果

下载：http://121.5.53.246/DTDesigner2.0.zip


# English introduction DUIThink

1. DUIThink is not just a UI library with an interface editor



2. At the same time, it is a PC platform frame for rapid prototyping of the project



3, it is an ecosystem, users can customize the development of controls in the DUIThink editor quick call, but also can be uploaded to the DUIThink platform to form a public all users can use the control, the control author can obtain user sponsorship.



Official website: http://121.5.53.246/



QQ group: 885314879

SourceCode：https://github.com/miaomiaoyule/DUIThink

Video tutorial: https://space.bilibili.com/322458488?spm_id_from=333.788.b_765f7570696e666f.1



Original Intention:



Before using duilib to do projects, you have to write xml, compared to MFC to develop gorgeous UI does improve a lot of efficiency, but writing xml is still inefficient and painful, and does not form your perfect ecosystem.



The reason is that duilib has a high learning cost for beginners, handwritten xml and code make a project take a long time to form, and since each company has its own magic version of open source, it is impossible to form a perfect ecosystem like U3D, cocos and layaer. In addition, some programmers who have developed for one or two years can not be flexible enough to drive them to choose the ui to be thrown to the front end, which increases labor costs and increases program complexity and volume, thus promoting the use of the front end to do ui, the application of dui is shrinking, and it is difficult for beginners to learn without the guidance of old programmers, which makes this situation worse. So developed this ui library with editor, and made a video tutorial, it can quickly help your project shape, when you master it can save you three quarters of the time and trouble.



Advantages:



1, carry flexible and efficient designer, can drag and drop, align controls, preview all properties of controls, modify properties in real time, and can view control events like MFC have automatically added form classes, control variables, event mapping functions



2, the designer can press Shift+Up\Down real-time adjustment control Z order, and real-time preview effect



3, the designer real-time preview of all picture resources, font resources, color resources, convenient attribute control, select control background, font, color resources no longer blindly rely on the name



4, the designer can carry out the menu level 1, 2, 3... Edit, and real-time popup level 2, level 3... Menus, you can edit them in real time



5, the designer supports Ctrl+C, Ctrl+V, Ctrl+Z, Ctrl+Y, easily Copy the view to another layout, and support undo, redo



6, the designer supports the developer custom control, and can preview all the properties of the control and real-time preview of the modification effect, the basic control in the designer supported by the operation of the custom control are also supported



7, you can upload your extension controls to the platform, so that other developers can use your controls, and you can receive their sponsorship



8. Pack items with one click in the designer



9, perfect support for svg pictures, perfect display like a browser (many software can not be perfectly supported, such as wps picture, some open source svg libraries, etc.)



10, perfect DPI support, real-time preview of each DPI effect in the designer



11, the powerful control Clone interface, you can directly Clone an identical control, if you need to duplicate the control, then it is suitable for you



12, powerful control Attach, Dettach interface, controls can be mounted to the parent node



13, powerful RichText drawing, a text can be divided into different fonts in different colors, and support left and right on top and down and line feed



14, powerful self-drawing Edit control, support to insert pictures, GIFs, text transparent channels, etc., can do chat message editing box



15, powerful Combox controls, support gradient subitem sliding, can do QQ login to select the account special effects



16, powerful Animate controls, support Gif and other GIFs, also support sequence frames



17, powerful ListView control, modify a property to achieve the List, Grid, TileH, TileV mode switch between, support subitems fixed height, non-fixed height of millions of quantities of silk sliding, support the free drag between subitems to exchange positions. Support List, Grid, TileH, TileV types within the control seamless switching, if you need to be content from List to Tile between arbitrary switching, then it is your first choice.



18, powerful tree controls, with selected button tree, no selected button tree, with table head tree, radio tree, etc., can complete the UI by modifying the properties in the designer



19, powerful rich form special effects, and control water ripple effect



20, support one-click skin function, you only need to set the skin in the editor, a line of code can skin



21, clear control event distribution and message notification



22, support control custom Bitmap background



23, the control layout is flexible around the dock, and according to the text dynamic calculation of the width and height of the control, according to the child control dynamic calculation of the width and height of the parent container, when you want the control to be the length of the text, or the width and height of the control is the sum of the width and height of the child control then you need it



Function description is not complete, please go to experience in detail



Use:



Development tools: VS2017 and above, the lower version of VS is not supported
Preparation: Compile Build.bat under ThirdDepend first
Start Designer: Compile the Release_Design mode DUIThink module, compile the Release_Unicode mode DUIPreview module, Copy DUIPreview.exe, DUIThink_Design.dll, DUIThink.dll, and MMHelper.dll to the designer run directory

Designer: video tutorial at https://space.bilibili.com/322458488?spm_id_from=333.788.b_765f7570696e666f.1

QQ group, official website also has video introduction
Problem solving: Please provide Demo and description of the problem, which can be written in CSDN and sent to my email 284168136@qq.com, I will solve it regularly, you can also send it to QQ group to discuss
