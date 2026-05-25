AIO.Register("core.ready", function(_, data)
  AIO.state.ready = true
  AIO.state.server = data
  AIO.state.capabilities = {}
  if data.capabilities then
    for cap in string.gmatch(data.capabilities, "([^,]+)") do
      AIO.state.capabilities[#AIO.state.capabilities + 1] = cap
    end
  end
  AIO_Event.Fire("READY", data)
end)

AIO.Register("core.pong", function(_, data)
  AIO_Event.Fire("PONG", data)
end)

AIO.Register("core.capabilities", function(_, data)
  AIO.state.capabilities = {}
  if data.list then
    for cap in string.gmatch(data.list, "([^,]+)") do
      AIO.state.capabilities[#AIO.state.capabilities + 1] = cap
    end
  end
  AIO_Event.Fire("CAPABILITIES", AIO.state.capabilities)
end)

AIO.Register("core.notify", function(_, data)
  local text = data.text or data.msg or ""
  local color = data.color or "ffffff"
  if text ~= "" then
    print(string.format("|cff%s%s|r", color, text))
  end
end)

AIO.On("LOGIN", function()
  AIO.Send("core.ping", "")
end)
