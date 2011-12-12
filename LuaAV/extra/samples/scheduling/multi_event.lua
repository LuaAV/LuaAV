
-- waits for the soonest of any condition
wait_earliest = (function()
	local condition_prefix = os.time()
	local condition_id = 1		-- private upvalue for safety
	return function(...)
		local conditions = {...}
		local unique_event = string.format("cond_%d_%d", condition_prefix, condition_id)
		condition_id = condition_id + 1
		
		-- schedule each condition to fire event(unique_event)
		for i, cond in ipairs(conditions) do
			go(cond, event, unique_event)
		end
	
		-- wait for one of them to fire
		wait(unique_event)
	end
end)()	

-- Here's a quick test.Ê

function test(id)
	local timeout = 0.5
	local event_id = "test_" .. id
	
	go(math.random(), event, event_id)
	
	-- wait until either event_id fires, but give up after timeout seconds:
	wait_earliest(event_id, timeout)
	
	if now() < 0.5 then
		print("event fired", event_id, now())
	else
		print("timeout fired", event_id)
	end
end

math.randomseed(os.time())
for i = 1, 10 do go(test, i) end