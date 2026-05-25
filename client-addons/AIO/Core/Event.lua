local Event = { _listeners = {} }
AIO_Event = Event

function Event.On(name, fn)
  local bucket = Event._listeners[name]
  if not bucket then
    bucket = {}
    Event._listeners[name] = bucket
  end
  bucket[#bucket + 1] = fn
  return fn
end

function Event.Off(name, fn)
  local bucket = Event._listeners[name]
  if not bucket then
    return
  end
  for i = #bucket, 1, -1 do
    if bucket[i] == fn then
      table.remove(bucket, i)
    end
  end
end

function Event.Fire(name, ...)
  local bucket = Event._listeners[name]
  if not bucket then
    return
  end
  for i = 1, #bucket do
    bucket[i](...)
  end
end

return Event
