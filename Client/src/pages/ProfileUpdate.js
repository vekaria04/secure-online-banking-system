import React, { useState } from "react";
import axios from "axios";
import { Container, TextField, Button, Typography, Alert, Box } from "@mui/material";
import { useNavigate } from "react-router-dom";

const BASE = process.env.REACT_APP_API_BASE_URL || "";
function ProfileUpdate() {
  const [name, setName] = useState("");
  const [email, setEmail] = useState("");
  const [message, setMessage] = useState("");
  const navigate = useNavigate();
  const token = localStorage.getItem("token");
  const userId = localStorage.getItem("userId");

  async function handleSubmit(e) {
    e.preventDefault();
    setMessage("");
    try {
      await axios.post(
        `${BASE}/update-profile`,
        {
          userId: parseInt(userId),
          name,
          email
        },
        {
          headers: {
            Authorization: `Bearer ${token}`
          }
        }
      );
      setMessage("Profile updated successfully");
      navigate("/dashboard");
    } catch (err) {
      setMessage("Profile update failed");
    }
  }

  return (
    <Container maxWidth="sm" sx={{ mt: 4 }}>
      <Typography variant="h4" gutterBottom>Update Profile</Typography>
      {message && <Alert severity="info" sx={{ mb: 2 }}>{message}</Alert>}
      <Box component="form" onSubmit={handleSubmit} sx={{ display: "flex", flexDirection: "column", gap: 2 }}>
        <TextField label="Name" value={name} onChange={(e) => setName(e.target.value)} required />
        <TextField label="Email" type="email" value={email} onChange={(e) => setEmail(e.target.value)} required />
        <Button variant="contained" type="submit">Update Profile</Button>
        <Button variant="outlined" onClick={() => navigate("/dashboard")}>Back to Dashboard</Button>
      </Box>
    </Container>
  );
}

export default ProfileUpdate;
