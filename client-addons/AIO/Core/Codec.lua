local Codec = {}
AIO_Codec = Codec

function Codec.Encode(tbl)
  if type(tbl) ~= "table" then
    return tostring(tbl or "")
  end
  local keys = {}
  for k in pairs(tbl) do
    keys[#keys + 1] = k
  end
  table.sort(keys)
  local parts = {}
  for i = 1, #keys do
    local k = keys[i]
    parts[#parts + 1] = k .. "=" .. tostring(tbl[k])
  end
  return table.concat(parts, ";")
end

function Codec.Decode(payload)
  if not payload or payload == "" then
    return {}
  end
  local out = {}
  for pair in string.gmatch(payload, "([^;]+)") do
    local k, v = pair:match("^([^=]+)=(.*)$")
    if k then
      out[k] = v
    end
  end
  return out
end

return Codec
