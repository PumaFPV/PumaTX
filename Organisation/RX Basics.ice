{
  "version": "1.2",
  "package": {
    "name": "",
    "version": "",
    "description": "",
    "author": "",
    "image": ""
  },
  "design": {
    "board": "icezum",
    "graph": {
      "blocks": [
        {
          "id": "5dbdba92-cc6d-4a56-8f7b-166c9341eb29",
          "type": "basic.input",
          "data": {
            "name": "RC Link",
            "pins": [
              {
                "index": "0",
                "name": "",
                "value": ""
              }
            ],
            "virtual": true,
            "clock": false
          },
          "position": {
            "x": 104,
            "y": 200
          }
        },
        {
          "id": "351f8528-934a-47de-9b79-50f6c88a077b",
          "type": "basic.output",
          "data": {
            "name": "FPort",
            "pins": [
              {
                "index": "0",
                "name": "",
                "value": ""
              }
            ],
            "virtual": true
          },
          "position": {
            "x": 648,
            "y": 200
          }
        },
        {
          "id": "eb2960ad-069a-4d12-9650-ae4968102f28",
          "type": "basic.output",
          "data": {
            "name": "Telemetry",
            "pins": [
              {
                "index": "0",
                "name": "",
                "value": ""
              }
            ],
            "virtual": true
          },
          "position": {
            "x": 648,
            "y": 248
          }
        }
      ],
      "wires": [
        {
          "source": {
            "block": "5dbdba92-cc6d-4a56-8f7b-166c9341eb29",
            "port": "out"
          },
          "target": {
            "block": "351f8528-934a-47de-9b79-50f6c88a077b",
            "port": "in"
          }
        },
        {
          "source": {
            "block": "5dbdba92-cc6d-4a56-8f7b-166c9341eb29",
            "port": "out"
          },
          "target": {
            "block": "eb2960ad-069a-4d12-9650-ae4968102f28",
            "port": "in"
          }
        }
      ]
    }
  },
  "dependencies": {}
}