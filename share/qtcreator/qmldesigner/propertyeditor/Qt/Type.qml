import Qt 4.6
import Bauhaus 1.0

GroupBox {
    id: type;
    finished: finishedNotify;
    caption: "Type";

    layout: VerticalLayout {
        spacing: 6
        QWidget {
            layout: HorizontalLayout {
                Label {
                    text: "type";
                    windowTextColor: isBaseState ? "#000000" : "#FF0000";
                }

                QLabel {
                    text: backendValues.className.value;
                }

            }
        }
        QWidget {
            property var isEnabled: isBaseState
            onIsEnabledChanged: idLineEdit.setStyleSheet("color: "+(isEnabled?scheme.defaultColor:scheme.disabledColor));
            ColorScheme{ id:scheme }

            layout: HorizontalLayout {
                Label {
                    text: "Id";
                }

                QLineEdit {
                    enabled: baseStateFlag
                    id: idLineEdit;
                    objectName: "idLineEdit";
                    readOnly: isBaseState != true;
                    text: backendValues.id === undefined ? "" : backendValues.id.value;
                    onEditingFinished: {
                        backendValues.id.value = text;
                    }
                }
            }
        }
    }
}
