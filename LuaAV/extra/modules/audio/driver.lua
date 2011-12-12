-- This file is for documentation purposes only

--- Interfacing with and configuring the audio driver
-- audio.driver.inputs and audio.driver.outputs store the available input/output devices (device name as key, parameter table as value)
-- @name audio.driver
module(...)

--- refresh the driver lists
-- audio.driver.inputs and audio.driver.outputs store the available input/output devices
function refresh() end

--- returns a string of status info from the driver
-- @ret string
function status() end

--- configure the audio driver
-- all argument fields are optional
-- @param args driver parameters
-- @param args.samplerate number
-- @param args.blocksize integer (should be a multiple of 64)
-- @param args.inputs integer 
-- @param args.outputs integer 
-- @param args.input device name
-- @param args.output device name 
-- @ret success, errormsg
function configure(args) end