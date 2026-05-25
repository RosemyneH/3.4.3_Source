-- Copy this file to register a new AIO module.
AIO.Module.Define("example", function(mod)
  AIO.Register("example.echo", function(_, data)
    print("|cff88ccffAIO Example|r echo:", data.text or "")
  end)

  function mod.Ping()
    AIO.SendTable("example.ping", { text = "hello from client" })
  end

  SLASH_AIOEX1 = "/aioex"
  SlashCmdList.AIOEX = function()
    mod.Ping()
  end
end)
