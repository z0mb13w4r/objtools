dicom_protocol = Proto("dicom-a",  "DICOM A-Type message")

pdu_type = ProtoField.uint8(
  "dicom-a.pdu_type",
  "pduType",
  base.DEC,
  {
    [1]="ASSOC Request",
    [2]="ASSOC Accept",
    [3]=”ASSOC Reject”,
    [4]=”Data”,
    [5]=”RELEASE Request”,
    [6]=”RELEASE Response”,
    [7]=”ABORT”
  }
)

message_length = ProtoField.uint16("dicom-a.message_length", "messageLength", base.DEC)

dicom_protocol.fields = {message_length, pdu_type} 1

function dicom_protocol.dissector(buffer, pinfo, tree)
  pinfo.cols.protocol = dicom_protocol.name
  local subtree = tree:add(dicom_protocol, buffer(), "DICOM PDU")
  subtree:add_le(pdu_type, buffer(0,1))
  subtree:add(message_length, buffer(2,4))
end

local tcp_port = DissectorTable.get("tcp.port")
tcp_port:add(4242, dicom_protocol)

