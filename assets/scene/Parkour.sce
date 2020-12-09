{
    "Scene": {
        "ptr_wrapper": {
            "id": 2147483649,
            "data": {
                "value0": 3,
                "value1": 0,
                "value2": {
                    "Tag": "Entity 0",
                    "Id": {
                        "uuid": "ac35ad59-766c-4d51-acbd-6fe78cbf51c4"
                    }
                },
                "value3": 1,
                "value4": {
                    "Tag": "Entity 1",
                    "Id": {
                        "uuid": "5a4e871d-f2c5-421d-adf9-06d89d560741"
                    }
                },
                "value5": 2,
                "value6": {
                    "Tag": "camera",
                    "Id": {
                        "uuid": "1dbf600e-aaf9-4526-b5c6-eb125efeb59d"
                    }
                },
                "value7": 3,
                "value8": 0,
                "value9": {
                    "translate": {
                        "x": -0.053950875997543338,
                        "y": 1.2337872982025147,
                        "z": -0.10000000149011612
                    },
                    "scale": {
                        "x": 1.0,
                        "y": 1.0,
                        "z": 1.0
                    },
                    "rotate": 0.0
                },
                "value10": 1,
                "value11": {
                    "translate": {
                        "x": 0.0,
                        "y": -1.0,
                        "z": 0.0
                    },
                    "scale": {
                        "x": 20.0,
                        "y": 1.0,
                        "z": 1.0
                    },
                    "rotate": 0.0
                },
                "value12": 2,
                "value13": {
                    "translate": {
                        "x": 0.0,
                        "y": 0.0,
                        "z": 0.0
                    },
                    "scale": {
                        "x": 8.88888931274414,
                        "y": 5.0,
                        "z": 1.0
                    },
                    "rotate": 0.0
                },
                "value14": 2,
                "value15": 0,
                "value16": {
                    "Color": {
                        "x": 1.0,
                        "y": 1.0,
                        "z": 1.0,
                        "w": 1.0
                    },
                    "Texture": {
                        "ptr_wrapper": {
                            "id": 2147483650,
                            "data": {
                                "path": "assets/texture/1.png",
                                "width": 712,
                                "height": 820,
                                "flip": false,
                                "option": {
                                    "minFilter": 2,
                                    "magFilter": 2,
                                    "wrapS": 0,
                                    "wrapT": 0
                                }
                            }
                        }
                    },
                    "UseTexture": true,
                    "UseAsSubTexture": false,
                    "setting": {
                        "type": 0,
                        "leftUpper": {
                            "x": 0.0,
                            "y": 0.0
                        },
                        "size": {
                            "x": 0.0,
                            "y": 0.0
                        },
                        "pos": {
                            "x": 0.0,
                            "y": 0.0
                        },
                        "cellSize": {
                            "x": 0.0,
                            "y": 0.0
                        },
                        "spriteGridSize": {
                            "x": 1.0,
                            "y": 1.0
                        }
                    },
                    "tiling": 1.0
                },
                "value17": 1,
                "value18": {
                    "Color": {
                        "x": 1.0,
                        "y": 1.0,
                        "z": 1.0,
                        "w": 1.0
                    },
                    "Texture": {
                        "ptr_wrapper": {
                            "id": 2147483651,
                            "data": {
                                "path": "assets/texture/1.png",
                                "width": 712,
                                "height": 820,
                                "flip": false,
                                "option": {
                                    "minFilter": 2,
                                    "magFilter": 2,
                                    "wrapS": 0,
                                    "wrapT": 0
                                }
                            }
                        }
                    },
                    "UseTexture": false,
                    "UseAsSubTexture": false,
                    "setting": {
                        "type": 0,
                        "leftUpper": {
                            "x": 0.0,
                            "y": 0.0
                        },
                        "size": {
                            "x": 0.0,
                            "y": 0.0
                        },
                        "pos": {
                            "x": 0.0,
                            "y": 0.0
                        },
                        "cellSize": {
                            "x": 0.0,
                            "y": 0.0
                        },
                        "spriteGridSize": {
                            "x": 1.0,
                            "y": 1.0
                        }
                    },
                    "tiling": 1.0
                },
                "value19": 1,
                "value20": 2,
                "value21": {
                    "camera": {
                        "value0": {
                            "projection": {
                                "col0": {
                                    "x": 0.22499999403953553,
                                    "y": 0.0,
                                    "z": 0.0,
                                    "w": 0.0
                                },
                                "col1": {
                                    "x": 0.0,
                                    "y": 0.4000000059604645,
                                    "z": 0.0,
                                    "w": 0.0
                                },
                                "col2": {
                                    "x": 0.0,
                                    "y": 0.0,
                                    "z": -0.0010000000474974514,
                                    "w": 0.0
                                },
                                "col3": {
                                    "x": -0.0,
                                    "y": -0.0,
                                    "z": -0.0,
                                    "w": 1.0
                                }
                            }
                        },
                        "orthoSize": 5.0,
                        "orthoNear": -1000.0,
                        "orthoFar": 1000.0,
                        "aspect": 1.7777777910232545
                    },
                    "lockAspect": true,
                    "primary": true
                },
                "value22": 0,
                "value23": 1,
                "value24": 0,
                "value25": {
                    "instance": {
                        "polymorphic_id": 2147483649,
                        "polymorphic_name": "rl::ParkourPlayerController",
                        "ptr_wrapper": {
                            "id": 2147483652,
                            "data": {
                                "walkAccel": 5.0,
                                "walkSpeedLimit": 5.0,
                                "jumpSpeed": 7.0
                            }
                        }
                    },
                    "script_name": "rl::ParkourPlayerController",
                    "valid": false
                },
                "value26": 2,
                "value27": 0,
                "value28": {
                    "angularVelocity": 0.0,
                    "angle": 0.0,
                    "velocity": {
                        "x": 0.0,
                        "y": 0.0
                    },
                    "friction": 0.20000000298023225,
                    "mass": 10.0,
                    "force": {
                        "x": 0.0,
                        "y": 0.0
                    },
                    "torque": 0.0,
                    "keepingForce": false,
                    "attachPoint": {
                        "x": 0.0,
                        "y": 0.0
                    },
                    "showAttachPoint": false,
                    "RestrictRotation": false
                },
                "value29": 1,
                "value30": {
                    "angularVelocity": 0.0,
                    "angle": 0.0,
                    "velocity": {
                        "x": 0.0,
                        "y": 0.0
                    },
                    "friction": 0.20000000298023225,
                    "mass": 3.4028234663852887e38,
                    "force": {
                        "x": 0.0,
                        "y": 0.0
                    },
                    "torque": 0.0,
                    "keepingForce": false,
                    "attachPoint": {
                        "x": 0.0,
                        "y": 0.0
                    },
                    "showAttachPoint": false,
                    "RestrictRotation": false
                },
                "value31": 2,
                "value32": 0,
                "value33": {
                    "x": 0.0,
                    "y": 0.0,
                    "w": 1.0,
                    "h": 1.0
                },
                "value34": 1,
                "value35": {
                    "x": 0.0,
                    "y": 0.0,
                    "w": 20.0,
                    "h": 1.0
                },
                "value36": 0,
                "value37": 0,
                "value38": 0,
                "value39": 0,
                "value40": 0,
                "value41": 0
            }
        }
    }
}