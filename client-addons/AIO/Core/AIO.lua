local PREFIX = "AIO"
local DELIM = "\t"
local VERSION = "1.0.0"

local Codec = AIO_Codec
local Event = AIO_Event

AIO = AIO or {}
AIO.VERSION = VERSION
AIO.PREFIX = PREFIX
AIO.handlers = AIO.handlers or {}
AIO.modules = AIO.modules or {}
AIO.state = AIO.state or {
  ready = false,
  capabilities = {},
  server = {},
}

local function splitMessage(msg)
  local delimPos = msg:find(DELIM, 1, true)
  if not delimPos then
    return msg, ""
  end
  return msg:sub(1, delimPos - 1), msg:sub(delimPos + 1)
end

function AIO.Register(handler, callback)
  assert(type(handler) == "string" and handler ~= "", "handler name required")
  assert(type(callback) == "function", "callback required")
  AIO.handlers[handler] = callback
end

function AIO.Unregister(handler)
  AIO.handlers[handler] = nil
end

function AIO.Send(handler, payload)
  if not UnitName("player") then
    return false
  end
  local body = handler .. DELIM .. (payload or "")
  SendAddonMessage(body, PREFIX, "WHISPER", UnitName("player"))
  return true
end

function AIO.SendTable(handler, tbl)
  return AIO.Send(handler, Codec.Encode(tbl))
end

function AIO.On(event, callback)
  return Event.On(event, callback)
end

function AIO.Module.Define(name, initFn)
  assert(type(name) == "string" and name ~= "", "module name required")
  assert(type(initFn) == "function", "module init required")
  if AIO.modules[name] then
    return AIO.modules[name]
  end
  local mod = { name = name }
  AIO.modules[name] = mod
  initFn(mod)
  Event.Fire("MODULE_LOADED", name, mod)
  return mod
end

local bus = CreateFrame("Frame")
bus:RegisterEvent("CHAT_MSG_ADDON")
bus:RegisterEvent("PLAYER_LOGIN")
bus:SetScript("OnEvent", function(_, event, ...)
  if event == "CHAT_MSG_ADDON" then
    local prefix, message, _, sender = ...
    if prefix ~= PREFIX or sender ~= UnitName("player") then
      return
    end
    local handler, payload = splitMessage(message)
    local fn = AIO.handlers[handler]
    if fn then
      fn(payload, Codec.Decode(payload))
    else
      Event.Fire("UNHANDLED", handler, payload)
    end
    return
  end

  if event == "PLAYER_LOGIN" then
    Event.Fire("LOGIN")
    AIO.Send("core.handshake", Codec.Encode({ version = VERSION }))
  end
end)

Event.On("READY", function()
  print(string.format("|cff00ff00AIO|r v%s ready — capabilities: %s", VERSION, table.concat(AIO.state.capabilities, ", ")))
end)

return AIO
