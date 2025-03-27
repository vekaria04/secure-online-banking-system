import React, { useState } from "react";
import axios from "axios";
import { Container, TextField, Button, Typography, Alert, Box } from "@mui/material";
import { useNavigate } from "react-router-dom";


function UpdatePassword() {
    const [oldPass, setOldPass] = useState("");
    const [newPass, setNewPass] = useState("");
    const [message, setMessage] = useState("");
    const navigate = useNavigate();
    const userId = localStorage.getItem("userId");


    async function handleSubmit(e) {
        e.preventDefault();
        setMessage("");
        try {
            const token = localStorage.getItem("token");
            await axios.post(
                "http://localhost:3001/update-password",
                {
                    userId: parseInt(userId),
                    oldPass,
                    newPass
                },
                {
                    headers: {
                        Authorization: `Bearer ${token}`
                    }
                }
            );
            setMessage("Password updated successfully");
            navigate("/dashboard");
        } catch (err) {
            setMessage("Password update failed");
        }
    }


    return (
        <Container maxWidth="sm" sx={{ mt: 4 }}>
            <Typography variant="h4" gutterBottom>Update Password</Typography>
            {message && <Alert severity="info" sx={{ mb: 2 }}>{message}</Alert>}
            <Box component="form" onSubmit={handleSubmit} sx={{ display: "flex", flexDirection: "column", gap: 2 }}>
                <TextField label="Old Password" type="password" value={oldPass} onChange={(e) => setOldPass(e.target.value)} required />
                <TextField label="New Password" type="password" value={newPass} onChange={(e) => setNewPass(e.target.value)} required />
                <Button variant="contained" type="submit">Update Password</Button>
                <Button variant="outlined" onClick={() => navigate("/dashboard")}>Back to Dashboard</Button>
            </Box>
        </Container>
    );
}


export default UpdatePassword;
