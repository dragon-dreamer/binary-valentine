.pragma library

class UrlHelper {
    constructor(urls) {
        if (urls.constructor.name !== "Array") {
            this.urls = [ urls ];
        } else {
            this.urls = urls;
        }

        this.urls = this.urls.map((url) => url.toString());
    }

    areLocalFiles() {
        return this.urls.every((url) => url.startsWith("file:///"));
    }

    getLocalFilePaths() {
        return this.urls.map((url) => UrlHelper.getLocalFilePath(url));
    }

    static getLocalFilePath(url) {
        var pureUrl;
        if (url.length > 9) {
            var cutPos = url.charAt(9) === ':' ? 8 : 7;
            pureUrl = url.substring(cutPos);
        } else {
            pureUrl = url;
        }
        pureUrl = decodeURIComponent(pureUrl);

        if (Qt.platform.os === "windows") {
            pureUrl = pureUrl.replace(/\\/g, '/');
        }
        return pureUrl;
    }
};
