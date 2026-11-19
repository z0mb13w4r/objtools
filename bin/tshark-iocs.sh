#!/bin/bash
PCAP="${1:-investigation.pcap}"
OUT="${2:-evidence/iocs}"

mkdir -p "$OUT/http_objects"

# External destination IPs
tshark -r "$PCAP" -q -T fields -e ip.dst \
  -Y "not ip.dst in {10.0.0.0/8 172.16.0.0/12 192.168.0.0/16}" \
  | sort -u > "$OUT/external_ips.txt"

# HTTP hostnames
tshark -r "$PCAP" -q -T fields -e http.host \
  -Y "http.request" | sort -u | grep -v '^$' > "$OUT/http_hosts.txt"

# TLS SNI values
tshark -r "$PCAP" -q -T fields -e tls.handshake.extensions_server_name \
  -Y "tls.handshake.type == 1" | sort -u | grep -v '^$' > "$OUT/tls_sni.txt"

# DNS queries
tshark -r "$PCAP" -q -T fields -e dns.qry.name \
  -Y "dns.flags.response == 0" | sort -u | grep -v '^$' > "$OUT/dns_queries.txt"

# JA4 fingerprints
tshark -r "$PCAP" -q -T fields -e tls.handshake.ja4 \
  -Y "tls.handshake.type == 1" | sort -u | grep -v '^$' > "$OUT/ja4_fingerprints.txt"

# All HTTP-transferred objects, then hash them
tshark -r "$PCAP" --export-objects http,"$OUT/http_objects/" >/dev/null 2>/dev/null
( cd "$OUT/http_objects" && sha256sum * 2>/dev/null > "$OUT/extracted_hashes.txt" )

echo "IOC extraction complete. Output in: $OUT"

