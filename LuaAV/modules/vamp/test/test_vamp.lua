require "vamp"
local plugins = vamp.plugins


for k, v in pairs(vamp) do
	print("vamp", k, v)
end

local paths = vamp.paths
for i, p in ipairs(paths) do
	print("vamp search path:", i, p)
end

function plugin_dump(plugin)
	print(string.rep("-", 80))
	print("name:", plugin.name)
	print("key:", plugin.key)
	print("description:", plugin.description)
	print("maker:", plugin.maker)
	print("copyright:", plugin.copyright)
	print("plugin_version:", plugin.plugin_version)
	print("API_version:", plugin.API_version)
	print("path:", plugin.path)
	print("index:", plugin.index)
	print()
	print("input_domain:", plugin.input_domain)
	print("min_channels:", plugin.min_channels)
	print("max_channels:", plugin.max_channels)
	print("step_size:", plugin.step_size)
	print("block_size:", plugin.block_size)
	for i, p in ipairs(plugin.params) do	
		print("\n\tparam", i)
		
		print("\t", "name:", p.name)
		print("\t", "identifier:", p.identifier)
		print("\t", "description:", p.description)
		
		print("\t", "unit:", p.unit)
		print("\t", "minValue:", p.minValue)
		print("\t", "maxValue:", p.maxValue)
		print("\t", "defaultValue:", p.defaultValue)
		if p.isQuantized then
			print("\t\t", "quantizeStep:", p.quantizeStep)
		end
		for j = 1, #p do
			if p[j] ~= "" then 
				print("\t\t", j, p[j])
			end
		end
	end
	
	for i, p in ipairs(plugin.outputs) do		
		print("\n\toutput", i)
		print("\t", "name:", p.name)
		print("\t", "identifier:", p.identifier)
		print("\t", "description:", p.description)
		print("\t", "unit:", p.unit)
		
		print("\t", "hasFixedBinCount:", p.hasFixedBinCount)
		if p.hasFixedBinCount then
			print("\t\t", "binCount:", p.binCount)
		end
		for j = 1, p.binCount do
			if p[j] ~= "" then 
				print("\t\t", j, p[j])
			end
		end
		print("\t", "hasKnownExtents:", p.hasKnownExtents)
		if p.hasKnownExtents then
			print("\t\t", "minValue:", p.minValue)
			print("\t\t", "maxValue:", p.maxValue)
		end
		print("\t", "isQuantized:", p.isQuantized)
		if p.isQuantized then
			print("\t\t", "quantizeStep:", p.quantizeStep)
		end
		print("\t", "sampleType:", p.sampleType)
		if p.sampleType ~= "OneSamplePerStep" then
			print("\t\t", "sampleRate:", p.sampleRate)
		end
		print("\t", "hasDuration:", p.hasDuration)
	end
	print(string.rep("-", 80))
end

for i, plugin in pairs(plugins) do
	print("plugin", i)
	plugin_dump(plugin)
end

if #plugins == 0 then
	print[=[
		No plugins found
		Perhaps you would like to install some from http://vamp-plugins.org/download.html
	]=]
else
	local plugin = plugins[1].load()
	print(plugin)
end

