AIO.Module.Define("dev", function(mod)
  mod.panel = nil

  function mod.TogglePanel()
    if mod.panel and mod.panel:IsShown() then
      mod.panel:Hide()
      return
    end
    if not mod.panel then
      mod.panel = CreateFrame("Frame", "AIODevPanel", UIParent)
      mod.panel:SetSize(280, 120)
      mod.panel:SetPoint("TOP", 0, -120)
      mod.panel:SetBackdrop({
        bgFile = "Interface\\DialogFrame\\UI-DialogBox-Background",
        edgeFile = "Interface\\DialogFrame\\UI-DialogBox-Border",
        tile = true, tileSize = 32, edgeSize = 16,
        insets = { left = 4, right = 4, top = 4, bottom = 4 },
      })
      mod.label = mod.panel:CreateFontString(nil, "OVERLAY", "GameFontHighlight")
      mod.label:SetPoint("TOP", 0, -12)
      mod.label:SetWidth(250)
      mod.label:SetJustifyH("LEFT")
    end
    mod.panel:Show()
    mod.Refresh()
  end

  function mod.Refresh()
    if not mod.label then
      return
    end
    local s = AIO.state.server
    mod.label:SetText(string.format(
      "|cff00ff00AIO Dev|r\nbuild=%s realm=%s\nportal=%s caps=%s",
      s.build or "?",
      s.realm or "?",
      s.portal or "?",
      table.concat(AIO.state.capabilities, ", ")
    ))
  end

  AIO.Register("dev.localhost", function(_, data)
    AIO.state.server.portal = data.portal or AIO.state.server.portal
    AIO.state.server.realm = data.realm or AIO.state.server.realm
    AIO_Event.Fire("LOCALHOST", data)
    if mod.panel and mod.panel:IsShown() then
      mod.Refresh()
    end
  end)

  AIO.On("READY", function()
    mod.Refresh()
  end)

  SLASH_AIODEV1 = "/aiodev"
  SlashCmdList.AIODEV = function()
    mod.TogglePanel()
  end
end)
