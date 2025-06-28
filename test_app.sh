#!/bin/bash

set -euo pipefail

PORT=8888
LONG_URL="https://example.com/some/long/url"
RESPONSE=$(curl -s -X POST -d "$LONG_URL" "http://localhost:$PORT/shorten")

echo "Server response: $RESPONSE"

SHORT_URL=$(echo "$RESPONSE" | grep -oP '(?<="short_url": ")[^"]+')

if [[ -z "$SHORT_URL" ]]; then
    echo "Failed to extract short URL"
    exit 1
fi

echo "Short URL: $SHORT_URL"

curl -s -o /dev/null -w "\nHTTP Code: %{http_code}\nRedirects to: %{redirect_url}\n" -L --max-redirs 0 "$SHORT_URL" || true
