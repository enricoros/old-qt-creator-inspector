import Qt 4.6
import Bauhaus 1.0

GroupBox {
    id: standardTextGroupBox

    caption: "Text";

    property bool showStyleColor: false;

    layout: VerticalLayout {
        QWidget {
            layout: HorizontalLayout {

                Label {
                    text: "Text"
                }

                LineEdit {
                    backendValue: backendValues.text
                    baseStateFlag: isBaseState;
                }
            }
        }
        QWidget {
            layout: HorizontalLayout {
                Label {
                    text: ""
                }
                CheckBox {
                    baseStateFlag: isBaseState;
                    text: "Is Wrapping";
                    checkable: true;
                    backendValue: backendValues.wrap;
                }
            }
        }

        QWidget {
            layout: HorizontalLayout {

                Label {
                    text: "Alignment"
                }
                AlignmentHorizontalButtons {}
                AlignmentVerticalButtons {}
            }
        }

        ColorLabel {
            text: "    Color"
        }

        ColorGroupBox {

            finished: finishedNotify

            backendColor: backendValues.color
        }

        ColorLabel {
            visible: showStyleColor
            text: "    Style Color"
        }

        ColorGroupBox {
            visible: showStyleColor
            finished: finishedNotify

            backendColor: backendValues.styleColor || null
        }

    }
}
