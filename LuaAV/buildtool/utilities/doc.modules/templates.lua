local io = io
local script = script


module(...)

header_image = "LuaAV.header.png"
html_template = io.open(script.path.."/doc.templates/doc.template.html"):read("*a")
html_content_template = io.open(script.path.."/doc.templates/doc.content.template.html"):read("*a")
navtree_template = io.open(script.path.."/doc.templates/navtree.template.js"):read("*a")
search_template = io.open(script.path.."/doc.templates/search.template.js"):read("*a")

active_tab_template = [[<li class="current"><a href="$filename"><span>$name</span></a></li>]]
tab_template = [[<li><a href="$filename"><span>$name</span></a></li>]]

reference_pane = [[
<!-- Pane Contents -->
<div class="contents">
@overview
<table class="memberdecls">
@sections @end

</table>
@end

<!-- Details -->
@details
<a name="details" id="details"></a>
<h2>Detailed Description</h2>
$description

@sections @end

@end
</div> <!-- contents -->
]]

section_overview = [[
<tr><td colspan="2">
	<h2>
		<!-- TOC Anchor -->
		<a name="$link"></a>$title
	</h2>
</td></tr>

@items @end
]]

section_overview_item = [[
<!-- A Documented Item -->
<tr>
	<!-- Item Name / Function or Macro Signature + link to specific page or expanded description on current page -->
	<td class="memItemRight" valign="bottom">$signature</td>
</tr>
<!-- Short Description, cutoff at a certain point (first sentence) -->
<tr>
	<td class="mdescRight">$description<br/></td>
</tr>
]]


section_detail = [[
<hr/>
<!-- Section Details -->
<h2>$title</h2>
@items @end

]]


item_detail = [[
<!-- Detailed Item (Complex Function Example) -->
@item
<a class="anchor" id="$hash"></a>
<div class="memitem">
	<div class="memproto">
@header
		@signature @end
@end
	</div>
	<div class="memdoc">
@body
	<!-- Text About Item -->
	$text
	
	<!-- Parameter Documentation -->
	@parameters @end

	<!-- Return Values -->
	@return_values @end
	
	<!-- Remarks -->
	@remarks @end
	
	<!-- See Also -->
	@seealso @end
@end
	</div>
</div>
@end
]]


attribute_signature = [[
	<table class="memname">
	<tr>
		<td class="memname">$name</td>
		<td></td>
		<td>$LuaMethod</td>
	</tr>
	</table>
]]

single_parameter_signature = [[
	<table class="memname">
	<tr>
		<td class="memname">$name</td>
		<td>(</td>
		@firstparameter @end
		<td>)</td>
		<td></td>
	</tr>
	</table>
]]

parameter_name_no_comma = [[
<td class="paramname"><em>$name</em></td>
]]


multi_parameter_signature = [[
<table class="memname">
	<tr>
		<td class="memname">$name</td>
		<td>(</td>
		@firstparameter @end
	</tr>
	@parameters @end
	<tr>
		<td></td>
		<td>)</td>
		<td></td>
	</tr>
</table>
]]

parameter_name_comma = [[
<td class="paramname"><em>$name</em>, </td>
]]

multi_parameter_signature_parameter_nth = [[
<tr>
	<td class="paramkey"></td>
	<td></td>
	$param
</tr>
]]



parameters = [[
<dl><dt><b>Parameters:</b></dt><dd>
	<table class="params">
		@parameters @end
	</table>
</dd>
</dl>
]]

parameter = [[
<tr><td class="paramname">$name</td><td>$description</td></tr>
]]



return_values = [[
<dl class="return"><dt><b>Returns:</b></dt>
	@return_values @end
</dl>
]]

return_value = [[
<dd>$description</dd>
]]




seealso = [[
<dl class="see"><dt><b>See also:</b></dt>
	@seealso @end
</dl>
]]


seealso_item = [[
<dd><a class="el" href="$link" title="$title">$name</a></dd>
]]



search_item = [[
<div class="SRResult" id="SR_$escaped_name">
	<div class="SREntry">
		<a id="Item$idx" onkeydown="return searchResults.Nav(event,$idx)" onkeypress="return searchResults.Nav(event,$idx)" onkeyup="return searchResults.Nav(event,$idx)" class="SRSymbol" href="$link" target="_parent">$name</a>
	</div>
</div>
]]

search_item_scoped = [[
<div class="SRResult" id="SR_$escaped_name">
	<div class="SREntry">
		<a id="Item$idx" onkeydown="return searchResults.Nav(event,$idx)" onkeypress="return searchResults.Nav(event,$idx)" onkeyup="return searchResults.Nav(event,$idx)" class="SRSymbol" href="$link" target="_parent">$name</a>
		<span class="SRScope">$scope</span>
	</div>
</div>
]]

search_item_tree = [[
<div class="SRResult" id="SR_$escaped_name">
	<div class="SREntry">
		<a id="Item$idx" onkeydown="return searchResults.Nav(event,$idx)" onkeypress="return searchResults.Nav(event,$idx)" onkeyup="return searchResults.Nav(event,$idx)" class="SRSymbol" href="javascript:searchResults.Toggle('SR_$name')">flags</a>
		<div class="SRChildren">
			@children @end
		</div>
	</div>
</div>
]]

search_item_children = [[
<a id="Item$idx{}_c$idx2{}" onkeydown="return searchResults.NavChild(event,$idx{},$idx2{})" onkeypress="return searchResults.NavChild(event,$idx{},$idx2{})" onkeyup="return searchResults.NavChild(event,$idx{},$idx2{})" class="SRScope" href="$link" target="_parent">$name</a>
]]

search_page = [[
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html><head><title></title>
<meta http-equiv="Content-Type" content="text/xhtml;charset=UTF-8"/>
<link rel="stylesheet" type="text/css" href="search.css"/>
<script type="text/javascript" src="search.js"></script>
</head>
<body class="SRPage">
<div id="SRIndex">
<div class="SRStatus" id="Loading">Loading...</div>

@items @end

<div class="SRStatus" id="Searching">Searching...</div>
<div class="SRStatus" id="NoMatches">No Matches</div>
<script type="text/javascript"><!--
document.getElementById("Loading").style.display="none";
document.getElementById("NoMatches").style.display="none";
var searchResults = new SearchResults("searchResults");
searchResults.Search();
--></script>
</div>
</body>
</html>
]]


--[[
<dl class="remark"><dt><b>Remarks:</b></dt>
		<dd>
			Max attribute syntax is used to define key-value pairs. For example, 
			<!-- Code Fragment -->
			<div class="fragment">
			<pre class="fragment">
				<span class="stringliteral">&quot;@key1 value @key2 another_value&quot;</span>
			</pre>
			</div>
		</dd>

		<dd>
			The example below creates a new object that in a patcher whose object pointer is stored in a variable called "aPatcher". 
			<div class="fragment">
			<pre class="fragment">
				<a class="code" href="structt__dictionary.html" title="The dictionary object.">t_dictionary</a> *d;
    			<a class="code" href="structt__object.html" title="The structure for the head of any object which wants to have inlets or outlets, or support attributes...">t_object</a> *o;
				<span class="keywordtype">char</span> text[4];
    
				<a class="code" href="group__misc.html#ga0022303fac866c8f5757aa56b67ce29d" title="Copy the contents of one string to another, in a manner safer than the standard strcpy() or strncpy()...">strncpy_zero</a>(text, <span class="stringliteral">&quot;foo&quot;</span>, 4);

				d = <a class="code" href="group__dictionary.html#ga5099d74946c0117f96e8fc0164efcdf2" title="Create a new dictionary populated with values using a combination of attribute and sprintf syntax...">dictionary_sprintf</a>(<span class="stringliteral">&quot;@maxclass comment @varname _name \</span>
<span class="stringliteral">        @text \&quot;%s\&quot; @patching_rect %.2f %.2f %.2f %.2f \</span>
<span class="stringliteral">        @fontsize %f @textcolor %f %f %f 1.0 \</span>
<span class="stringliteral">        @fontname %s @bgcolor 0.001 0.001 0.001 0.&quot;</span>,
        text, 20.0, 20.0, 200.0, 24.0,
        18, 0.9, 0.9, 0.9, <span class="stringliteral">&quot;Arial&quot;</span>);
    
   	 			o = <a class="code" href="group__obj.html#gaed2c4e1d0c80d929b97ccf07a886faeb" title="Place a new object into a patcher.">newobject_fromdictionary</a>(aPatcher, d);
			</pre><
			/div>
		</dd>
	</dl>
--]]