import QtQuick 1.0
import com.nokia.symbian 1.0


Page{
    id: filespage
    property string filepath;
    property variant model;
    property alias listmodel: files_list_view.model;
    property bool first: true;
    tools: ToolBarLayout {
        id: filesTools
        ToolButton { iconSource: "toolbar-back"; onClicked: pageStack.pop() }
        ToolButton{ iconSource: "toolbar-home";onClicked: {
                pageStack.clear();
                pageStack.push(mainPage);
            }}
        ToolButton { iconSource: "toolbar-add"; onClicked: {
                window.addFiles(filepath);
                console.log("FILEPATH:"+filepath);
            } }
    }
    ListView{
        id: files_list_view
        delegate: filesDelegate
        anchors { left: parent.left; right: parent.right; top: headingrect.bottom; bottom: parent.bottom }
        clip: true
        spacing: 2
    }
    Rectangle {
        id:headingrect
        anchors {left:parent.left;right:parent.right;}
        height: artext.height
        color: Qt.rgba(0.07, 0.07, 0.07, 1)
        Text{
            id: artext
            text: (filepath===""? "Files:" : filepath+":")
            color: "white"
            font.pointSize: 7
        }
    }

    onStatusChanged: {
        console.debug("Playlist status changed: "+status);
        if(status==PageStatus.Activating)
        {
            if(first==false)
            {
                console.debug("File  reactivating: requesting path: "+filepath);
                window.requestFilesModel((filepath=="")? "/":filepath);
            //filesClicked((filepath=="")? "/":filepath);
            }
            first = false;
        }
    }

    Component{
        id:filesDelegate
        Item {
            id: itemItem
            width: window.width
            height: topLayout.height+liststretch
            Rectangle {
                color: (isDirectory===true) ? Qt.rgba(0.14, 0.14, 0.14, 1) : Qt.rgba(0.07, 0.07, 0.07, 1)
                anchors.fill: parent
                Text{
                    id: topLayout
                    anchors {verticalCenter: parent.verticalCenter}
                    Text { text: name; color:"white";font.pointSize:8}
                    //Text {text:artist; color:"grey";font.pointSize:10;}
                }
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if(isDirectory){
                        list_view1.currentIndex = index
                        console.log("File: "+prepath+name+" clicked");
                        filesClicked((prepath=="/"? "": prepath+"/")+name);
                    }
                    if(isFile) {
                        console.debug("File clicked: "+title+":"+album);
                        albumTrackClicked(title,album,artist,length,path,year,tracknr);
                    }
                }
                onPressAndHold: {
                        filesMenu.filepath = (prepath=="/"? "": prepath+"/")+name;
                    filesMenu.directory = isDirectory;
                    if(isFile){
                        filesMenu.title = title;
                        filesMenu.album = album;
                        filesMenu.artist = artist;
                        filesMenu.length = length;
                        filesMenu.year = year;
                        filesMenu.nr = tracknr;
                    }
                        filesMenu.open();

                }
            }
        }
    }

    ScrollBar
    {
        id:filescrollbar
        flickableItem: files_list_view
        anchors {right:files_list_view.right; top:files_list_view.top}
    }

    ContextMenu {
        id: filesMenu
        property string filepath;
        property string title;
        property string album;
        property string artist;
        property string length;
        property string year;
        property int nr;
        property bool directory:false;
        MenuLayout {
            MenuItem {
                text: "Show song information"
                visible: !filesMenu.directory
                onClicked: {
                    albumTrackClicked(filesMenu.title,filesMenu.album,filesMenu.artist,filesMenu.length,filesMenu.filepath,filesMenu.year,filesMenu.nr);
                }
            }
            MenuItem {
                text: filesMenu.directory ?  "Add directory" : "Add file"
                onClicked: {
                    window.addFiles(filesMenu.filepath);
                }
            }
            MenuItem {
                text: filesMenu.directory ?  "Playback directory" : "Playback file"
                onClicked: {
                           window.playSong(filesMenu.filepath);
                }
            }
        }
    }

}