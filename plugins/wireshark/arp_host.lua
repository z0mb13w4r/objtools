-- document as you go
local arp_host_info = {
  version = "1.0.0",
  author = "Kyle Wickens",
  descriptor = "Arp IP Target - resolved",
  repository = "https://github.com/z0mb13w4r/objtools/plugins/wireshark"
}

set_plugin_info(arp_host_info)

-- add some field(s) to the protocol
local pf = {
  target_host = ProtoField.string("arp_host.target", "ARP target (resolved)")
}

-- create a protocol to attach new fields to
local arp_host_p = Proto.new("arp_host", "Arp IP Target - resolved")

arp_host_p.fields = pf

-- create a field extractor to copy packet field data
local arp_target_f = Field.new("arp.dst.proto_ipv4")

-- create the postdissector function that will run on each frame/packet
function arp_host_p.dissector(tvb,pinfo,tree)
  local subtree = nil
  
  -- copy existing field(s) into table for processing
  finfo = { arp_target_f() }
  
  if (#finfo > 0) then
    if not subtree then
      subtree = tree:add(arp_host_p)
    end
    
    for k, v in pairs(finfo) do
      -- process data and add results to the tree
      subtree:add(pf.target_host, v.display)
    end
  end
end

-- register the new protocol as a postdissector
register_postdissector(arp_host_p)

