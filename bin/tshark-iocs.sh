#!/bin/bash

SCRIPTVERSION=1.0
SCRIPTNAME=$(basename "$0")

PCAP="${1:-investigation.pcap}"
OUT="${2:-evidence/iocs}"

GREPBIN=/usr/bin/grep
SORTBIN=/usr/bin/sort
MKDIRBIN=/usr/bin/mkdir
TSHARKBIN=/usr/bin/tshark
SHA256SUMBIN=/usr/bin/sha256sum

${MKDIRBIN} -p "${OUT}/http_objects"

# External destination IPs
${TSHARKBIN} -r "${PCAP}" -q -T fields -e ip.dst \
  -Y "not ip.dst in {10.0.0.0/8 172.16.0.0/12 192.168.0.0/16}" \
  | ${SORTBIN} -u > "${OUT}/external_ips.txt"

# HTTP hostnames
${TSHARKBIN} -r "${PCAP}" -q -T fields -e http.host \
  -Y "http.request" | ${SORTBIN} -u | ${GREPBIN} -v '^$' > "${OUT}/http_hosts.txt"

# TLS SNI values
${TSHARKBIN} -r "${PCAP}" -q -T fields -e tls.handshake.extensions_server_name \
  -Y "tls.handshake.type == 1" | ${SORTBIN} -u | ${GREPBIN} -v '^$' > "${OUT}/tls_sni.txt"

# DNS queries
${TSHARKBIN} -r "${PCAP}" -q -T fields -e dns.qry.name \
  -Y "dns.flags.response == 0" | ${SORTBIN} -u | ${GREPBIN} -v '^$' > "${OUT}/dns_queries.txt"

# JA4 fingerprints
${TSHARKBIN} -r "${PCAP}" -q -T fields -e tls.handshake.ja4 \
  -Y "tls.handshake.type == 1" | ${SORTBIN} -u | ${GREPBIN} -v '^$' > "${OUT}/ja4_fingerprints.txt"

# All HTTP-transferred objects, then hash them
${TSHARKBIN} -r "${PCAP}" --export-objects http,"${OUT}/http_objects/" >/dev/null 2>/dev/null
( cd "${OUT}/http_objects" && ${SHA256SUMBIN} * 2>/dev/null > "${OUT}/extracted_hashes.txt" )

echo "IOC extraction complete. Output in: ${OUT}"

