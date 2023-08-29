# Chinese introduction DUIThink
1、DUIThink不仅是一款带界面编辑器的UI库

2、同时是一款使项目快速成型的一个PC端平台框架

3、更是一个生态，用户可自定义开发控件在DUIThink编辑器快速调用，更可上传到DUIThink平台形成一个公共的所有使用者都可以使用的控件，控件作者可以获得使用者赞助。

官网：http://www.DUIThink.com/

QQ群：885314879

视频教程：https://space.bilibili.com/322458488?spm_id_from=333.788.b_765f7570696e666f.1

源码：https://github.com/miaomiaoyule/DUIThink

初衷：
  
        之前使用duilib做项目，都要手写xml，相比MFC开发绚丽的UI确实提升了不少效率，但是手写xml还是效率低下且让你痛苦至极，且没有形成你个完美的生态圈。
  
        原因是duilib对于初学者学习成本高，手写xml加代码让一个项目成型时期较长，且由于开源基本各个公司都有自己的魔改版本，所以根本就无法像U3D、cocos、layaer形成一个完美的生态圈，加之一些开发一两年的程序员也无法灵活驾驭促使他们会选择ui丢给前端做，既增加了人工成本又增加了程序复杂度以及体积，从而促使使用前端做ui水涨船高，dui的适用面愈发缩减，新手在没有老程序员指点的情况下很难学会，从而使这种情况雪上加霜，所以开发了这款带编辑器的ui库，且做了视频教程，它可以快速帮你的项目成型，当你掌握了它可以帮你省去四分之三的时间以及麻烦。

优点：

        1、携带灵活高效的设计器，可任意拖放、对齐控件，预览控件所有属性，修改属性实时生效，且可以像MFC一样查看控件事件已经自动添加窗体类、控件变量、事件映射函数

        2、设计器内可按Shift+Up\Down实时调整控件Z序，且实时预览效果

        3、设计器内实时预览所有图片资源、字体资源、颜色资源，便捷的属性控制，选择控件背景、字体、颜色资源不再只盲目依靠名字

        4、设计器内可进行菜单一级、二级、三级...编辑，且设计器内实时popup二级、三级...菜单，你可以实时编辑它们

        5、设计器内支持Ctrl+C、Ctrl+V、Ctrl+Z、Ctrl+Y，轻松Copy视图到另一个布局里，且支持撤销、重做

        6、设计器支持开发者自定义控件，且可预览控件所有属性以及实时预览修改效果，基础控件在设计器内所支持的操作自定义控件也都支持

        7、你可将你的扩展控件上传平台，这样其他开发者也可以使用你的控件，并且你可以收到他们的赞助

        8、设计器内一键打包项目

        9、完美支持svg图片，像浏览器一样完美显示（很多软件都无法完美支持，例如wps看图、一些开源的svg库等）

        10、完美DPI支持，设计器内实时预览各DPI效果

        11、强大的控件Clone接口，可以直接Clone一个一模一样的控件，如果你的需求里重复的复制控件，那么它适合你

        12、强大的控件Attach、Dettach接口，控件可以随意挂载父节点

        13、强大的RichText绘制，一条文本可以分不同的字体不同颜色绘制，且支持靠左靠右靠上靠下以及换行

        14、强大的自绘Edit控件，支持插入图片、动图、文字透明通道等，可做聊天消息编辑框

        15、强大的Combox控件，支持梯度子项滑动，可做QQ登录选择账号时的特效

        16、强大的Animate控件，支持Gif等动图，也支持序列帧

        17、强大的ListView控件，修改一个属性即可实现List、Grid、TileH、TileV模式之间切换，支持子项固定高度、非固定高度的百万级量丝滑滑动，支持子项间随意拖动交换位置，支持控件内List、Grid、TileH、TileV类型无缝切换，如果你的需求是内容从List到Tile间任意切换，那么它是你的首选。

        18、强大的树控件，带选中按钮树、无选中按钮树、带表头树、单选树等在设计器通过修改属性即可完成UI

        19、强大丰富的窗体特效、以及控件水波纹效果

        20、支持一键换肤功能，你只需要在编辑器设置皮肤，一行代码即可换肤

        21、清晰的控件事件派发以及消息通知

        22、支持控件自定义Bitmap背景

        23、控件布局灵活的左右上下停靠，以及依据文本动态计算控件宽高、依据子控件动态计算父容器宽高，当你想让控件是文本的长度，或者控件的宽高是子控件宽高之和那么你需要它

        功能介绍不完全，详细请去体验

使用：

        开发工具：VS2017以上，低版本VS暂未支持
        准备工作：先编译ThirdDepend下的Build.bat
        启动设计器：编译Release_Design模式DUIThink模块，编译Release_Unicode模式DUIPreview模块，把DUIPreview.exe、DUIThink_Design.dll、DUIThink.dll、MMHelper.dll拷贝到设计器运行目录即可
        使用设计器：视频教程https://space.bilibili.com/322458488?spm_id_from=333.788.b_765f7570696e666f.1
        QQ群、官网也有视频介绍
        问题解决：请提供问题Demo以及问题描述，描述可写在CSDN，发我邮箱284168136@qq.com，我会定期解决，你也可以发到QQ群里讨论

# English introduction DUIThink

1. DUIThink is not just a UI library with an interface editor



2. At the same time, it is a PC platform frame for rapid prototyping of the project



3, it is an ecosystem, users can customize the development of controls in the DUIThink editor quick call, but also can be uploaded to the DUIThink platform to form a public all users can use the control, the control author can obtain user sponsorship.



Official website: http://www.DUIThink.com/



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
