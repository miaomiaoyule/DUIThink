# Chinese introduction DUIThink
DUIThink是一个Windows下免费开源的DirectUI界面库，包含了一整套桌面软件的开发部件，与其说这是一个界面库，不如说它是一个开发框架，功能组件有（线程池、文件操作、iocp网络模型、文件监听、注册表监听、文件拖拽、命令行解析、磁吸算法、内存池、分辨率转换、字符串操作等）。您完全可以直接使用这个框架来开发任何你想要的桌面应用。  
  
官网：http://49.235.209.245/DUIThink    
QQ群：885314879   
视频教程：https://space.bilibili.com/322458488?spm_id_from=333.788.b_765f7570696e666f.1    
  
# 初衷：  
- mfc做不了绚丽的ui，很多地方需要自绘，修改起来也麻烦   
- 手写xml上手难，效率低下，预览调整也麻烦，非所见即所得，一些控件属性记不住还要对照属性表使用  

# DUIThink库：  
- 1、控件布局靠上、靠下、靠左、靠右、居中、相对、绝对   
- 2、控件大小固定宽高、最大最小宽高、根据文本自动调整宽高、根据子控件自动调整宽高   
- 3、控件背景颜色、图片、圆角、渐变颜色   
- 4、支持svg图片，比浏览器支持的格式多（很多软件都不完美支持，例如wps看图、一些开源的svg库等）   
- 5、支持dpi，dpi切换只需要一行代码   
- 6、控件Clone接口，可以复制出一模一样的view，如果你需要重复的view，那么它适合你   
- 7、控件Attach、Dettach接口，控件可以随意挂载父节点   
- 8、RichText绘制，一条文本可以分不同的字体、不同颜色渲染，且支持多行、靠左、靠右、靠上、靠下、居中   
- 9、自绘Edit控件，支持插入图片、动图、文字透明通道等，可做聊天消息编辑框   
- 10、自绘ip控件   
- 11、自绘日历控件   
- 12、3d旋转菜单控件  
- 13、Clock时钟控件  
- 14、Combox控件，支持梯度子项滑动，类似以前QQ登录选择账号时的特效  
- 15、Animate控件，支持Gif、序列帧  
- 16、ListView控件，修改一个属性即可实现List、Grid、TileH、TileV模式之间切换，支持子项间随意拖动交换位置  
- 17、树控件，带选中按钮树、无选中按钮树、带表头树、单选树修改属性即可完成切换  
- 18、丰富的窗体特效  
- 19、控件水波纹特效  
- 20、一键换肤功能，只需要一行代码  
- 21、控件自定义Bitmap背景  

# DTDesigner设计器（闭源）：  
- 1、真正所见即所得  
- 2、绑定vs的sln项目文件可以像mfc一样自动生成窗体类、控件变量、控件事件代码  
- 3、任意拖放、对齐控件，修改控件属性所见即所得  
- 4、图片、字体、颜色资源一览无余  
- 5、控件树视图Shift+Up\Down实时调整控件Z序  
- 6、实时进行菜单一级、二级、三级...编辑  
- 7、支持Ctrl+C、Ctrl+V、Ctrl+Z、Ctrl+Y  
- 8、复制ui到另一个视图里，无需重复制作ui  
- 9、支持开发者自定义控件  
- 10、实时预览窗体各dpi效果  
 
   
  
  
# English introduction DUIThink
DUIThink is a free and open source DirectUI interface library for Windows, which contains a complete set of desktop software development components. It is not so much an interface library as a development framework. The functional components are (thread pool, file operation, iocp network model, file listening, registry listening, file dragging, command line parsing, magnetic algorithm, memory pool, resolution conversion, string operation, etc.). You can use this framework directly to develop any desktop application you want.
* * *
Official website: http://49.235.209.245/DUIThink    
QQ group: 885314879  
Video tutorial: https://space.bilibili.com/322458488?spm_id_from=333.788.b_765f7570696e666f.1  

# Original Intention:  
- mfc can not do gorgeous ui, many places need to draw themselves, modify it is also troublesome  
- handwriting xml is inefficient, preview adjustment is also troublesome, what you see is what you get, some control properties can not remember but also use against the property table  

# DUIThink library:  
- 1、control layout on the top, bottom, left, right, center, relative, absolute  
- 2、the control size fixed width and height, maximum and minimum width and height, automatically adjust the width and height according to the text, automatically adjust the width and height according to the child control  
- 3、control background color, picture, rounded corner, gradient color  
- 4、support svg pictures, more formats than browsers support (many software is not perfect support, such as wps picture, some open source svg libraries, etc.)  
- 5、Support dpi. Only one line of code is required for dpi switching  
- 6、control Clone interface, can copy the same view, if you need to repeat the view, then it is suitable for you
- 7、the control Attach, Dettach interface, the control can be attached to the parent node
- 8、RichText drawing, a text can be divided into different fonts, different colors rendering, and support multiple lines, left, right, top, bottom, center
- 9、self-drawing Edit control, support to insert pictures, GIFs, text transparent channels, etc., can do chat message editing box
- 10、Self-drawing ip controls
- 11、self-drawing calendar control
- 12、3d rotation menu control
- 13、Clock control
- 14、Combox controls, support gradient subitem sliding, similar to the previous QQ login select account special effects
- 15、Animate controls, supporting Gif and sequence frames
- 16、ListView control, modify a property to achieve the List, Grid, TileH, TileV mode switch between, support the subitems at will drag exchange position
- 17、tree control, with selected button tree, no selected button tree, with table head tree, radio tree modify the properties to complete the switch
- 18、rich form effects
- 19、control water ripple effect
- 20、one-click skin function, only need a line of code
- 21、control custom Bitmap background

# DTDesigner (Closed source) :
- 1、like mfc automatically generate form classes, control variables, control event code
- 2、any drag and drop, align controls, modify control properties wysiWYG
- 3、picture, font, color resources at a glance
- 4、control tree view Shift+Up\Down real-time adjustment control Z order
- 5、real-time menu level 1, 2, 3... EDITOR
- 6、Support Ctrl+C, Ctrl+V, Ctrl+Z, and Ctrl+Y
- 7、Copy the ui to another view, no need to make the ui again
- 8、support developer custom controls
- 9、real-time preview of the dpi effect of the form  
