-- This file is for documentation purposes only

--- Interfacing with the LuaAV scheduler
-- @name LuaAV.time
module(...)

--- yield the containing coroutine to the scheduler
-- can be run in the main (top-level) script, but not safe within callbacks (such as Window:draw())
-- @param w duration (seconds) or event name (string)
function wait(w) end

--- resume all coroutines waiting on this event name
-- @param e event name (string)
function event(e) end

--- create a new independent scheduler (not connected to the real-time clock)
-- @ret scheduler scheduler object
function scheduler() end

--- schedule a function
-- @param w (optional) duration (seconds) or event name (string) to wait for before starting coroutine
-- @param func function to form the body of the coroutine
-- @param ... args passed to the coroutine
function go(w, func, ...) end

--- report the current CPU time
-- @return CPU time
function cpu() end

--- report the current script scheduler time
-- @ret time in seconds since script began
function now() end
