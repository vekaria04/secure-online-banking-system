const getBaseUrl = () => {
    if (process.env.NODE_ENV === "development") {
        return "";
    }
    return process.env.REACT_APP_API_BASE_URL;
};

export default getBaseUrl;