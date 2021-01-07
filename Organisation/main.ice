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
          "id": "48dab5d8-3ac3-4e27-9f5d-22c7b73b3b19",
          "type": "basic.output",
          "data": {
            "name": "UAV Response",
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
            "x": 1248,
            "y": 64
          }
        },
        {
          "id": "bf2af9ef-c987-4a5b-9f63-474fe89d2dc2",
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
            "x": 64,
            "y": 96
          }
        },
        {
          "id": "ab686b30-1bc2-4aed-ae55-fbae73f6b09d",
          "type": "basic.output",
          "data": {
            "name": "Display",
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
            "x": 632,
            "y": 128
          }
        },
        {
          "id": "6ac4218b-fe29-44a8-9cf2-24c98839c440",
          "type": "b0775bb06eb0d214cb39e1d60bd261c6c0463fc4",
          "position": {
            "x": 232,
            "y": 64
          },
          "size": {
            "width": 96,
            "height": 128
          }
        },
        {
          "id": "8f2a7926-7bea-482d-8288-4584ca73396e",
          "type": "bc8464b0c45cdd66193238a7cc1cda85139be481",
          "position": {
            "x": 472,
            "y": 64
          },
          "size": {
            "width": 96,
            "height": 128
          }
        },
        {
          "id": "c00a1efb-2daa-4e33-b621-fd0185c804a5",
          "type": "f3dc46dbabe1be81bed98c976a076deb4355f4bc",
          "position": {
            "x": 864,
            "y": 64
          },
          "size": {
            "width": 96,
            "height": 64
          }
        },
        {
          "id": "266e729a-6014-410a-bca1-4fad50582353",
          "type": "a168ca617efc220555a7da1ca9c86a7cf617870e",
          "position": {
            "x": 1096,
            "y": 64
          },
          "size": {
            "width": 96,
            "height": 64
          }
        },
        {
          "id": "27d743f4-2e23-4ee0-9d5a-8d35d5aa5ce8",
          "type": "basic.info",
          "data": {
            "info": "Tx",
            "readonly": false
          },
          "position": {
            "x": 472,
            "y": -16
          },
          "size": {
            "width": 80,
            "height": 64
          }
        },
        {
          "id": "b01170fb-fee6-49f5-9f2d-4c4faa1aaa0d",
          "type": "basic.info",
          "data": {
            "info": "Rx",
            "readonly": false
          },
          "position": {
            "x": 872,
            "y": -8
          },
          "size": {
            "width": 72,
            "height": 56
          }
        }
      ],
      "wires": [
        {
          "source": {
            "block": "bf2af9ef-c987-4a5b-9f63-474fe89d2dc2",
            "port": "out"
          },
          "target": {
            "block": "6ac4218b-fe29-44a8-9cf2-24c98839c440",
            "port": "965a2ad7-de06-47bc-b689-fe4b7f4fa556"
          }
        },
        {
          "source": {
            "block": "266e729a-6014-410a-bca1-4fad50582353",
            "port": "f22e075b-c108-4fdb-b8df-af535cae0c89"
          },
          "target": {
            "block": "48dab5d8-3ac3-4e27-9f5d-22c7b73b3b19",
            "port": "in"
          }
        },
        {
          "source": {
            "block": "c00a1efb-2daa-4e33-b621-fd0185c804a5",
            "port": "351f8528-934a-47de-9b79-50f6c88a077b"
          },
          "target": {
            "block": "266e729a-6014-410a-bca1-4fad50582353",
            "port": "57472f6b-dad2-4aa3-9b2b-af92f12a22c7"
          }
        },
        {
          "source": {
            "block": "c00a1efb-2daa-4e33-b621-fd0185c804a5",
            "port": "eb2960ad-069a-4d12-9650-ae4968102f28"
          },
          "target": {
            "block": "266e729a-6014-410a-bca1-4fad50582353",
            "port": "d9420237-80ce-4bfd-abee-884407a7b651"
          }
        },
        {
          "source": {
            "block": "8f2a7926-7bea-482d-8288-4584ca73396e",
            "port": "4de05155-d859-4331-9f82-12913f00592f"
          },
          "target": {
            "block": "c00a1efb-2daa-4e33-b621-fd0185c804a5",
            "port": "5dbdba92-cc6d-4a56-8f7b-166c9341eb29"
          }
        },
        {
          "source": {
            "block": "6ac4218b-fe29-44a8-9cf2-24c98839c440",
            "port": "9ef86aa6-aac3-4aa2-bff0-3cd0988c85f1"
          },
          "target": {
            "block": "8f2a7926-7bea-482d-8288-4584ca73396e",
            "port": "38c55c12-0eae-41e9-b272-909d7671dfb4"
          }
        },
        {
          "source": {
            "block": "6ac4218b-fe29-44a8-9cf2-24c98839c440",
            "port": "7a71019a-2b7e-456d-a362-f0ae61e7b99a"
          },
          "target": {
            "block": "8f2a7926-7bea-482d-8288-4584ca73396e",
            "port": "602ff507-238c-466f-99cc-bf97f527cc21"
          }
        },
        {
          "source": {
            "block": "6ac4218b-fe29-44a8-9cf2-24c98839c440",
            "port": "d179862a-1ab2-4d25-b427-3a405a280701"
          },
          "target": {
            "block": "8f2a7926-7bea-482d-8288-4584ca73396e",
            "port": "9fcf53bd-262d-4f8a-9426-92a1c165b270"
          }
        },
        {
          "source": {
            "block": "6ac4218b-fe29-44a8-9cf2-24c98839c440",
            "port": "f06caffa-fe9b-4606-af6e-fec92f7665cc"
          },
          "target": {
            "block": "8f2a7926-7bea-482d-8288-4584ca73396e",
            "port": "f60d7d1a-1c62-4831-a818-64dd2e76a43f"
          }
        },
        {
          "source": {
            "block": "8f2a7926-7bea-482d-8288-4584ca73396e",
            "port": "de4c0a6b-1ce3-4f9e-93b6-b98892f240e9"
          },
          "target": {
            "block": "ab686b30-1bc2-4aed-ae55-fbae73f6b09d",
            "port": "in"
          }
        }
      ]
    }
  },
  "dependencies": {
    "b0775bb06eb0d214cb39e1d60bd261c6c0463fc4": {
      "package": {
        "name": "",
        "version": "",
        "description": "",
        "author": "",
        "image": ""
      },
      "design": {
        "graph": {
          "blocks": [
            {
              "id": "9ef86aa6-aac3-4aa2-bff0-3cd0988c85f1",
              "type": "basic.output",
              "data": {
                "name": "Joystick"
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
                "name": "Switch"
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
                "name": "Pot"
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
                "name": "Menu Nav"
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
      }
    },
    "bc8464b0c45cdd66193238a7cc1cda85139be481": {
      "package": {
        "name": "",
        "version": "",
        "description": "",
        "author": "",
        "image": ""
      },
      "design": {
        "graph": {
          "blocks": [
            {
              "id": "38c55c12-0eae-41e9-b272-909d7671dfb4",
              "type": "basic.input",
              "data": {
                "name": "Joystick",
                "clock": false
              },
              "position": {
                "x": 152,
                "y": 128
              }
            },
            {
              "id": "602ff507-238c-466f-99cc-bf97f527cc21",
              "type": "basic.input",
              "data": {
                "name": "Switch",
                "clock": false
              },
              "position": {
                "x": 152,
                "y": 176
              }
            },
            {
              "id": "4de05155-d859-4331-9f82-12913f00592f",
              "type": "basic.output",
              "data": {
                "name": "RC Link"
              },
              "position": {
                "x": 640,
                "y": 176
              }
            },
            {
              "id": "9fcf53bd-262d-4f8a-9426-92a1c165b270",
              "type": "basic.input",
              "data": {
                "name": "Pot",
                "clock": false
              },
              "position": {
                "x": 152,
                "y": 224
              }
            },
            {
              "id": "f60d7d1a-1c62-4831-a818-64dd2e76a43f",
              "type": "basic.input",
              "data": {
                "name": "Menu Nav",
                "clock": false
              },
              "position": {
                "x": 152,
                "y": 272
              }
            },
            {
              "id": "de4c0a6b-1ce3-4f9e-93b6-b98892f240e9",
              "type": "basic.output",
              "data": {
                "name": "Display"
              },
              "position": {
                "x": 640,
                "y": 272
              }
            },
            {
              "id": "30e9de87-1342-4988-a6df-be684d70d518",
              "type": "2d00dc6cbf770622c583a0dff0e0d8e2763ab2ec",
              "position": {
                "x": 456,
                "y": 160
              },
              "size": {
                "width": 96,
                "height": 96
              }
            }
          ],
          "wires": [
            {
              "source": {
                "block": "38c55c12-0eae-41e9-b272-909d7671dfb4",
                "port": "out"
              },
              "target": {
                "block": "30e9de87-1342-4988-a6df-be684d70d518",
                "port": "c3f73f68-1074-4355-b69f-6a20f7bea3e7"
              }
            },
            {
              "source": {
                "block": "602ff507-238c-466f-99cc-bf97f527cc21",
                "port": "out"
              },
              "target": {
                "block": "30e9de87-1342-4988-a6df-be684d70d518",
                "port": "5fb29465-2ee7-45bb-afa4-9a3de895c774"
              }
            },
            {
              "source": {
                "block": "9fcf53bd-262d-4f8a-9426-92a1c165b270",
                "port": "out"
              },
              "target": {
                "block": "30e9de87-1342-4988-a6df-be684d70d518",
                "port": "67ed5e09-486d-4f97-929f-aefea9c43951"
              },
              "vertices": [
                {
                  "x": 432,
                  "y": 256
                }
              ]
            },
            {
              "source": {
                "block": "30e9de87-1342-4988-a6df-be684d70d518",
                "port": "061aa997-2f30-4591-8841-fb6abf5c3b2e"
              },
              "target": {
                "block": "4de05155-d859-4331-9f82-12913f00592f",
                "port": "in"
              }
            },
            {
              "source": {
                "block": "f60d7d1a-1c62-4831-a818-64dd2e76a43f",
                "port": "out"
              },
              "target": {
                "block": "de4c0a6b-1ce3-4f9e-93b6-b98892f240e9",
                "port": "in"
              }
            }
          ]
        }
      }
    },
    "2d00dc6cbf770622c583a0dff0e0d8e2763ab2ec": {
      "package": {
        "name": "Mux 2:1",
        "version": "1.0.0",
        "description": "Multiplexer 2:1",
        "author": "Carlos Diaz",
        "image": "%3Csvg%20xmlns=%22http://www.w3.org/2000/svg%22%20viewBox=%22-252%20400.9%2090%2040%22%3E%3Cpath%20d=%22M-162%20419.9h-24v-7.2l-41-11.8v9h-25v2h25v18h-25v2h25v9l41-11.7v-7.4h24v-1.9zm-63%2018.5v-35l37%2010.8v13.5l-37%2010.7z%22/%3E%3C/svg%3E"
      },
      "design": {
        "graph": {
          "blocks": [
            {
              "id": "c3f73f68-1074-4355-b69f-6a20f7bea3e7",
              "type": "basic.input",
              "data": {
                "name": "i0"
              },
              "position": {
                "x": 80,
                "y": 120
              }
            },
            {
              "id": "5fb29465-2ee7-45bb-afa4-9a3de895c774",
              "type": "basic.input",
              "data": {
                "name": "i1"
              },
              "position": {
                "x": 80,
                "y": 208
              }
            },
            {
              "id": "061aa997-2f30-4591-8841-fb6abf5c3b2e",
              "type": "basic.output",
              "data": {
                "name": "o"
              },
              "position": {
                "x": 792,
                "y": 208
              }
            },
            {
              "id": "67ed5e09-486d-4f97-929f-aefea9c43951",
              "type": "basic.input",
              "data": {
                "name": "sel"
              },
              "position": {
                "x": 80,
                "y": 296
              }
            },
            {
              "id": "ba573190-2ead-411a-a323-1b15a22d46db",
              "type": "basic.code",
              "data": {
                "code": "reg _o;\n\nalways @(*) begin\n    case(sel0)\n        0: _o = in0;\n        1: _o = in1;\n        default: _o = in0;\n    endcase\nend\n\nassign o = _o;",
                "params": [],
                "ports": {
                  "in": [
                    {
                      "name": "in0"
                    },
                    {
                      "name": "in1"
                    },
                    {
                      "name": "sel0"
                    }
                  ],
                  "out": [
                    {
                      "name": "o"
                    }
                  ]
                }
              },
              "position": {
                "x": 312,
                "y": 112
              },
              "size": {
                "width": 384,
                "height": 256
              }
            }
          ],
          "wires": [
            {
              "source": {
                "block": "ba573190-2ead-411a-a323-1b15a22d46db",
                "port": "o"
              },
              "target": {
                "block": "061aa997-2f30-4591-8841-fb6abf5c3b2e",
                "port": "in"
              }
            },
            {
              "source": {
                "block": "c3f73f68-1074-4355-b69f-6a20f7bea3e7",
                "port": "out"
              },
              "target": {
                "block": "ba573190-2ead-411a-a323-1b15a22d46db",
                "port": "in0"
              }
            },
            {
              "source": {
                "block": "5fb29465-2ee7-45bb-afa4-9a3de895c774",
                "port": "out"
              },
              "target": {
                "block": "ba573190-2ead-411a-a323-1b15a22d46db",
                "port": "in1"
              }
            },
            {
              "source": {
                "block": "67ed5e09-486d-4f97-929f-aefea9c43951",
                "port": "out"
              },
              "target": {
                "block": "ba573190-2ead-411a-a323-1b15a22d46db",
                "port": "sel0"
              }
            }
          ]
        }
      }
    },
    "f3dc46dbabe1be81bed98c976a076deb4355f4bc": {
      "package": {
        "name": "",
        "version": "",
        "description": "",
        "author": "",
        "image": ""
      },
      "design": {
        "graph": {
          "blocks": [
            {
              "id": "5dbdba92-cc6d-4a56-8f7b-166c9341eb29",
              "type": "basic.input",
              "data": {
                "name": "RC Link",
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
                "name": "FPort"
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
                "name": "Telemetry"
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
      }
    },
    "a168ca617efc220555a7da1ca9c86a7cf617870e": {
      "package": {
        "name": "",
        "version": "",
        "description": "",
        "author": "",
        "image": ""
      },
      "design": {
        "graph": {
          "blocks": [
            {
              "id": "57472f6b-dad2-4aa3-9b2b-af92f12a22c7",
              "type": "basic.input",
              "data": {
                "name": "FPort",
                "clock": false
              },
              "position": {
                "x": 128,
                "y": 144
              }
            },
            {
              "id": "f22e075b-c108-4fdb-b8df-af535cae0c89",
              "type": "basic.output",
              "data": {
                "name": "FC"
              },
              "position": {
                "x": 384,
                "y": 168
              }
            },
            {
              "id": "d9420237-80ce-4bfd-abee-884407a7b651",
              "type": "basic.input",
              "data": {
                "name": "Telemetry",
                "clock": false
              },
              "position": {
                "x": 128,
                "y": 192
              }
            },
            {
              "id": "9c73b714-9501-4a50-abea-64f55f73e64e",
              "type": "7ebc902cbb1c4db116741533a86182485900ecda",
              "position": {
                "x": 256,
                "y": 168
              },
              "size": {
                "width": 96,
                "height": 64
              }
            }
          ],
          "wires": [
            {
              "source": {
                "block": "57472f6b-dad2-4aa3-9b2b-af92f12a22c7",
                "port": "out"
              },
              "target": {
                "block": "9c73b714-9501-4a50-abea-64f55f73e64e",
                "port": "18c2ebc7-5152-439c-9b3f-851c59bac834"
              }
            },
            {
              "source": {
                "block": "d9420237-80ce-4bfd-abee-884407a7b651",
                "port": "out"
              },
              "target": {
                "block": "9c73b714-9501-4a50-abea-64f55f73e64e",
                "port": "97b51945-d716-4b6c-9db9-970d08541249"
              }
            },
            {
              "source": {
                "block": "9c73b714-9501-4a50-abea-64f55f73e64e",
                "port": "664caf9e-5f40-4df4-800a-b626af702e62"
              },
              "target": {
                "block": "f22e075b-c108-4fdb-b8df-af535cae0c89",
                "port": "in"
              }
            }
          ]
        }
      }
    },
    "7ebc902cbb1c4db116741533a86182485900ecda": {
      "package": {
        "name": "AND",
        "version": "1.0.0",
        "description": "AND logic gate",
        "author": "Jesús Arroyo",
        "image": "%3Csvg%20xmlns=%22http://www.w3.org/2000/svg%22%20viewBox=%22-252%20400.9%2090%2040%22%3E%3Cpath%20d=%22M-252%20409.9h26v2h-26zM-252%20429.9h27v2h-27z%22/%3E%3Cpath%20d=%22M-227%20400.9v39.9h20.4c11.3%200%2020-9%2020-20s-8.7-20-20-20H-227zm2.9%202.8h17.6c9.8%200%2016.7%207.6%2016.7%2017.1%200%209.5-7.4%2017.1-17.1%2017.1H-224c-.1.1-.1-34.2-.1-34.2z%22/%3E%3Cpath%20d=%22M-187.911%20419.9H-162v2h-25.911z%22/%3E%3C/svg%3E"
      },
      "design": {
        "graph": {
          "blocks": [
            {
              "id": "18c2ebc7-5152-439c-9b3f-851c59bac834",
              "type": "basic.input",
              "data": {
                "name": ""
              },
              "position": {
                "x": 64,
                "y": 80
              }
            },
            {
              "id": "664caf9e-5f40-4df4-800a-b626af702e62",
              "type": "basic.output",
              "data": {
                "name": ""
              },
              "position": {
                "x": 752,
                "y": 144
              }
            },
            {
              "id": "97b51945-d716-4b6c-9db9-970d08541249",
              "type": "basic.input",
              "data": {
                "name": ""
              },
              "position": {
                "x": 64,
                "y": 208
              }
            },
            {
              "id": "00925b04-5004-4307-a737-fa4e97c8b6ab",
              "type": "basic.code",
              "data": {
                "code": "// AND logic gate\n\nassign c = a & b;",
                "params": [],
                "ports": {
                  "in": [
                    {
                      "name": "a"
                    },
                    {
                      "name": "b"
                    }
                  ],
                  "out": [
                    {
                      "name": "c"
                    }
                  ]
                }
              },
              "position": {
                "x": 256,
                "y": 48
              },
              "size": {
                "width": 384,
                "height": 256
              }
            }
          ],
          "wires": [
            {
              "source": {
                "block": "18c2ebc7-5152-439c-9b3f-851c59bac834",
                "port": "out"
              },
              "target": {
                "block": "00925b04-5004-4307-a737-fa4e97c8b6ab",
                "port": "a"
              }
            },
            {
              "source": {
                "block": "97b51945-d716-4b6c-9db9-970d08541249",
                "port": "out"
              },
              "target": {
                "block": "00925b04-5004-4307-a737-fa4e97c8b6ab",
                "port": "b"
              }
            },
            {
              "source": {
                "block": "00925b04-5004-4307-a737-fa4e97c8b6ab",
                "port": "c"
              },
              "target": {
                "block": "664caf9e-5f40-4df4-800a-b626af702e62",
                "port": "in"
              }
            }
          ]
        }
      }
    }
  }
}