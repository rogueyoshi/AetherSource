local events = {}

function RegisterEvent (event)
	table.insert(events, event)
	return #events
end

function UnregisterEvent (eventIndex)
	table.remove(events, eventIndex)
end

function ProcessEvents ()
	for _, event in pairs(events) do
		if event.condition() then
			event.triggered = true
		else
			event.triggered = false
			event.before = false
		end
		
		if event.once then 
			if event.triggered and not event.before then
				event.callback()
				event.before = true
			end
		else
			event.callback()
		end
	end
end
