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
          "id": "9ef86aa6-aac3-4aa2-bff0-3cd0988c85f1",
          "type": "basic.output",
          "data": {
            "name": "Joystick",
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
            "x": 400,
            "y": 56
          }
        },
        {
          "id": "7a71019a-2b7e-456d-a362-f0ae61e7b99a",
          "type": "basic.output",
          "data": {
            "name": "Switch",
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
            "x": 400,
            "y": 104
          }
        },
        {
          "id": "965a2ad7-de06-47bc-b689-fe4b7f4fa556",
          "type": "basic.input",
          "data": {
            "name": "User Input",
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
            "x": 200,
            "y": 152
          }
        },
        {
          "id": "d179862a-1ab2-4d25-b427-3a405a280701",
          "type": "basic.output",
          "data": {
            "name": "Pot",
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
            "x": 400,
            "y": 152
          }
        },
        {
          "id": "f06caffa-fe9b-4606-af6e-fec92f7665cc",
          "type": "basic.output",
          "data": {
            "name": "Menu Nav",
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
            "x": 400,
            "y": 200
          }
        }
      ],
      "wires": [
        {
          "source": {
            "block": "965a2ad7-de06-47bc-b689-fe4b7f4fa556",
            "port": "out"
          },
          "target": {
            "block": "9ef86aa6-aac3-4aa2-bff0-3cd0988c85f1",
            "port": "in"
          },
          "vertices": [
            {
              "x": 384,
              "y": 144
            }
          ]
        },
        {
          "source": {
            "block": "965a2ad7-de06-47bc-b689-fe4b7f4fa556",
            "port": "out"
          },
          "target": {
            "block": "7a71019a-2b7e-456d-a362-f0ae61e7b99a",
            "port": "in"
          }
        },
        {
          "source": {
            "block": "965a2ad7-de06-47bc-b689-fe4b7f4fa556",
            "port": "out"
          },
          "target": {
            "block": "d179862a-1ab2-4d25-b427-3a405a280701",
            "port": "in"
          }
        },
        {
          "source": {
            "block": "965a2ad7-de06-47bc-b689-fe4b7f4fa556",
            "port": "out"
          },
          "target": {
            "block": "f06caffa-fe9b-4606-af6e-fec92f7665cc",
            "port": "in"
          }
        }
      ]
    }
  },
  "dependencies": {}
}