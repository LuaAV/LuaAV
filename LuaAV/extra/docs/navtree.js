var NAVTREE =
[
[ "LuaAV", "index.html", [
  [ "LuaAV Overview", "index.html", null ] ,
  [ "LuaAV Manual", "manual.html", [
    [ "Introduction to LuaAV", "tutorial.01.introduction.to.LuaAV.html", null ] ,
    [ "Introduction to Lua", "tutorial.02.introduction.to.Lua.html", null ] ,
    [ "Timing and Scheduling", "tutorial.03.timing.and.scheduling.html", null ] ,
    [ "Making Sound", "tutorial.04.audio.tutorial.1.html", null ] ,
    [ "Drawing Shapes", "tutorial.05.graphics.tutorial.1.html", null ] ,
    [ "Multi-Channel Audio", "tutorial.06.multi.channel.audio.html", null ] ,
    [ "Drawing with Images", "tutorial.07.multi.channel.audio.html", null ] 
  ] ] ,
  [ "LuaAV Reference", "reference.html", [
    [ "app", "app.html", [
      [ "app.Window", "app.Window.html", null ] ,
      [ "app.screens", "app.screens.html", null ] 
    ] ] ,
    [ "Array", "Array.html", null ] ,
    [ "audio", "audio.html", [
      [ "audio.Def", "audio.Def.html", null ] ,
      [ "audio.util", "audio.util.html", null ] ,
      [ "audio.driver", "audio.driver.html", null ] 
    ] ] ,
    [ "font", "font.html", [
      [ "font.Font", "font.Font.html", null ] 
    ] ] ,
    [ "gui", "gui.html", [
      [ "gui.Button", "gui.Button.html", null ] ,
      [ "gui.Context", "gui.Context.html", null ] ,
      [ "gui.draw", "gui.draw.html", null ] ,
      [ "gui.Label", "gui.Label.html", null ] ,
      [ "gui.Layer", "gui.Layer.html", null ] ,
      [ "gui.Rect", "gui.Rect.html", null ] ,
      [ "gui.slider", "gui.slider.html", null ] ,
      [ "gui.View", "gui.View.html", null ] 
    ] ] ,
    [ "Image", "Image.html", null ] ,
    [ "Label", "Label.html", null ] ,
    [ "LuaAV", "LuaAV.html", [
      [ "LuaAV.time", "LuaAV.time.html", null ] 
    ] ] ,
    [ "midi", "midi.html", [
      [ "midi.Input", "midi.Input.html", null ] ,
      [ "midi.Output", "midi.Output.html", null ] 
    ] ] ,
    [ "opencl", "opencl.html", [
      [ "opencl.Kernel", "opencl.Kernel.html", null ] 
    ] ] ,
    [ "opengl", "opengl.html", [
      [ "opengl.Camera", "opengl.Camera.html", null ] ,
      [ "opengl.Draw", "opengl.Draw.html", null ] ,
      [ "opengl.Mesh", "opengl.Mesh.html", null ] ,
      [ "opengl.Texture", "opengl.Texture.html", null ] ,
      [ "opengl.Shader", "opengl.Shader.html", null ] ,
      [ "opengl.ShaderAttribute", "opengl.ShaderAttribute.html", null ] ,
      [ "opengl.Slab", "opengl.Slab.html", null ] ,
      [ "opengl.sketch", "opengl.sketch.html", null ] ,
      [ "opengl.Tesselator", "opengl.Tesselator.html", null ] 
    ] ] ,
    [ "osc", "osc.html", [
      [ "osc.Recv", "osc.Recv.html", null ] ,
      [ "osc.Send", "osc.Send.html", null ] 
    ] ] ,
    [ "space", "space.html", [
      [ "space.Nav", "space.Nav.html", null ] ,
      [ "space.Space", "space.Space.html", null ] ,
      [ "space.vec3", "space.vec3.html", null ] ,
      [ "space.quat", "space.quat.html", null ] 
    ] ] ,
    [ "video", "video.html", [
      [ "video.Video", "video.Video.html", null ] ,
      [ "video.VideoCamera", "video.VideoCamera.html", null ] ,
      [ "video.VideoRecorder", "video.VideoRecorder.html", null ] 
    ] ] 
  ] ] 
] ] 
];

function createIndent(o,domNode,node,level)
{
  if (node.parentNode && node.parentNode.parentNode)
  {
    createIndent(o,domNode,node.parentNode,level+1);
  }
  var imgNode = document.createElement("img");
  if (level==0 && node.childrenData)
  {
    node.plus_img = imgNode;
    node.expandToggle = document.createElement("a");
    node.expandToggle.href = "javascript:void(0)";
    node.expandToggle.onclick = function() 
    {
      if (node.expanded) 
      {
        $(node.getChildrenUL()).slideUp("fast");
        if (node.isLast)
        {
          node.plus_img.src = node.relpath+"ftv2plastnode.png";
        }
        else
        {
          node.plus_img.src = node.relpath+"ftv2pnode.png";
        }
        node.expanded = false;
      } 
      else 
      {
        expandNode(o, node, false);
      }
    }
    node.expandToggle.appendChild(imgNode);
    domNode.appendChild(node.expandToggle);
  }
  else
  {
    domNode.appendChild(imgNode);
  }
  if (level==0)
  {
    if (node.isLast)
    {
      if (node.childrenData)
      {
        imgNode.src = node.relpath+"ftv2plastnode.png";
      }
      else
      {
        imgNode.src = node.relpath+"ftv2lastnode.png";
        domNode.appendChild(imgNode);
      }
    }
    else
    {
      if (node.childrenData)
      {
        imgNode.src = node.relpath+"ftv2pnode.png";
      }
      else
      {
        imgNode.src = node.relpath+"ftv2node.png";
        domNode.appendChild(imgNode);
      }
    }
  }
  else
  {
    if (node.isLast)
    {
      imgNode.src = node.relpath+"ftv2blank.png";
    }
    else
    {
      imgNode.src = node.relpath+"ftv2vertline.png";
    }
  }
  imgNode.border = "0";
}

function newNode(o, po, text, link, childrenData, lastNode)
{
  var node = new Object();
  node.children = Array();
  node.childrenData = childrenData;
  node.depth = po.depth + 1;
  node.relpath = po.relpath;
  node.isLast = lastNode;

  node.li = document.createElement("li");
  po.getChildrenUL().appendChild(node.li);
  node.parentNode = po;

  node.itemDiv = document.createElement("div");
  node.itemDiv.className = "item";

  node.labelSpan = document.createElement("span");
  node.labelSpan.className = "label";

  createIndent(o,node.itemDiv,node,0);
  node.itemDiv.appendChild(node.labelSpan);
  node.li.appendChild(node.itemDiv);

  var a = document.createElement("a");
  node.labelSpan.appendChild(a);
  node.label = document.createTextNode(text);
  a.appendChild(node.label);
  if (link) 
  {
    a.href = node.relpath+link;
  } 
  else 
  {
    if (childrenData != null) 
    {
      a.className = "nolink";
      a.href = "javascript:void(0)";
      a.onclick = node.expandToggle.onclick;
      node.expanded = false;
    }
  }

  node.childrenUL = null;
  node.getChildrenUL = function() 
  {
    if (!node.childrenUL) 
    {
      node.childrenUL = document.createElement("ul");
      node.childrenUL.className = "children_ul";
      node.childrenUL.style.display = "none";
      node.li.appendChild(node.childrenUL);
    }
    return node.childrenUL;
  };

  return node;
}

function showRoot()
{
  var headerHeight = $("#top").height();
  var footerHeight = $("#nav-path").height();
  var windowHeight = $(window).height() - headerHeight - footerHeight;
  navtree.scrollTo('#selected',0,{offset:-windowHeight/2});
}

function expandNode(o, node, imm)
{
  if (node.childrenData && !node.expanded) 
  {
    if (!node.childrenVisited) 
    {
      getNode(o, node);
    }
    if (imm)
    {
      $(node.getChildrenUL()).show();
    } 
    else 
    {
      $(node.getChildrenUL()).slideDown("fast",showRoot);
    }
    if (node.isLast)
    {
      node.plus_img.src = node.relpath+"ftv2mlastnode.png";
    }
    else
    {
      node.plus_img.src = node.relpath+"ftv2mnode.png";
    }
    node.expanded = true;
  }
}

function getNode(o, po)
{
  po.childrenVisited = true;
  var l = po.childrenData.length-1;
  for (var i in po.childrenData) 
  {
    var nodeData = po.childrenData[i];
    po.children[i] = newNode(o, po, nodeData[0], nodeData[1], nodeData[2],
        i==l);
  }
}

function findNavTreePage(url, data)
{
  var nodes = data;
  var result = null;
  for (var i in nodes) 
  {
    var d = nodes[i];
    if (d[1] == url) 
    {
      return new Array(i);
    }
    else if (d[2] != null) // array of children
    {
      result = findNavTreePage(url, d[2]);
      if (result != null) 
      {
        return (new Array(i).concat(result));
      }
    }
  }
  return null;
}

function initNavTree(toroot,relpath)
{
  var o = new Object();
  o.toroot = toroot;
  o.node = new Object();
  o.node.li = document.getElementById("nav-tree-contents");
  o.node.childrenData = NAVTREE;
  o.node.children = new Array();
  o.node.childrenUL = document.createElement("ul");
  o.node.getChildrenUL = function() { return o.node.childrenUL; };
  o.node.li.appendChild(o.node.childrenUL);
  o.node.depth = 0;
  o.node.relpath = relpath;

  getNode(o, o.node);

  o.breadcrumbs = findNavTreePage(toroot, NAVTREE);
  if (o.breadcrumbs == null)
  {
    o.breadcrumbs = findNavTreePage("index.html",NAVTREE);
  }
  if (o.breadcrumbs != null && o.breadcrumbs.length>0)
  {
    var p = o.node;
    for (var i in o.breadcrumbs) 
    {
      var j = o.breadcrumbs[i];
      p = p.children[j];
      expandNode(o,p,true);
    }
    p.itemDiv.className = p.itemDiv.className + " selected";
    p.itemDiv.id = "selected";
    $(window).load(showRoot);
  }
}

