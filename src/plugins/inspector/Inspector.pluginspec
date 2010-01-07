<plugin name="Inspector" version="1.3.80" compatVersion="1.3.80">
    <vendor>Enrico Ros</vendor>
    <copyright>(C) 2009-2010 Enrico Ros</copyright>
    <license>
Commercial Usage

Licensees holding valid Qt Commercial licenses may use this plugin in accordance with the Qt Commercial License Agreement provided with the Software or, alternatively, in accordance with the terms contained in a written agreement between you and Nokia.

GNU Lesser General Public License Usage

Alternatively, this plugin may be used under the terms of the GNU Lesser General Public License version 2.1 as published by the Free Software Foundation.  Please review the following information to ensure the GNU Lesser General Public License version 2.1 requirements will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
    </license>
    <description>Runtime Probing plugin.</description>
     <url>http://www.enricoros.com</url>
    <dependencyList>
        <dependency name="TextEditor" version="1.3.80"/>
        <dependency name="ProjectExplorer" version="1.3.80"/>
        <dependency name="Core" version="1.3.80"/>
    </dependencyList>
    <argumentList>
        <argument name="-inspectoron">Enable inspector by default</argument>
        <argument name="-disable-slotmon-unused">Disable Signal/Slot monitoring</argument>
        <argument name="-disable-eloop-unused">Disable Event loop statistics</argument>
        <argument name="-disable-mmon-unused">Disable Memory allocations monitoring</argument>
    </argumentList>
</plugin>